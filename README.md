# OpenGL 3D Terrain Editor

Terrain renderer and editor.  

# Demo

(https://www.youtube.com/watch?v=yoSk325gkoc)

# Table of Contencts

[Libraries](#libraries)  
[How it works](#how-it-works)
  - [Terrain Mesh](#terrain-mesh)
  - [Height Map](#height-map)
  - [Normal Map](#normal-map)
  - [Blend Map](#blend-map)
  - [Level of Detail](#level-of-detail)
  - [Editing Terrain](#editing-terrain)

[End Note](#end-note)

## Libraries
[OpenGL](https://www.opengl.org/) Graphics library  
[gl3w](https://github.com/skaslev/gl3w) OpenGL core profile loading  
[GLFW](https://www.glfw.org/) OpenGL window creation and input handling  
[SOIL](https://github.com/littlstar/soil) OpenGL image library  
[glm](https://glm.g-truc.net/0.9.9/) OpenGL math library

## How it Works

### Terrain Mesh

The terrain mesh is built by rendering one tile x times, and a tile is built of 2 triangles with the following vertices.

![](https://github.com/willardt/3.31/blob/main/ss/tile.png?raw=true "")  

```C++
    constexpr GLfloat TILE_VERTICES[] = {
	  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f
  };
```

We take this mesh and draw it using OpenGL's instance rendering depending on the width and length of the terrain.

```C++
glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, TILE_VERTICES_SIZE / 2, node->_root->_width * node->_root->_length);
```
Here is the result of a flat terrain with simple shading:

![](https://github.com/willardt/3.31/blob/main/ss/terrain.png?raw=true "")  

### Height Map

The next step is to add height to the terrain mesh. This is done by creating an array of height values for each vertex on the mesh.
We dont need 6 height values per tile, we only need 4 since 2 vertices overlap. The size of the height array is calculated by (width + 1) * (length + 1).
The heights are maped per vertex row not per tile since there is overlap between vertices and tiles.

![](https://github.com/willardt/3.31/blob/main/ss/heights.png?raw=true "")

### Normal Map

Normals are stored the same way as the height map. To calculated the normal we first calculate the face normal for each triangle then average the normals from all adjacent triangles to a vertex.

We pass the height and normal map as a texture buffer to glsl and access the height and normal of any vertex with a function like this.

```glsl
const int vertex_indices[6] = {2, 0, 1, 2, 3, 1};
vec3 get_normal(const int vertex) {
	const int index = gl_InstanceID + (gl_InstanceID / width);

	switch(vertex_indices[vertex]) {
		case 0 :	return texelFetch(normals, index).xyz;				break;
		case 1 :	return texelFetch(normals, index + 1).xyz;			break;
		case 2 :	return texelFetch(normals, index + (width + 1)).xyz;		break;
		case 3 :	return texelFetch(normals, index + 1 + (width + 1)).xyz;	break;
	}
}
```


Here is the result after adding a height map and normal map:

![](https://github.com/willardt/3.31/blob/main/ss/terrain2.png?raw=true "")

### Blend Map

The next step is to add textures to the terrain. To do this we take 4 seperate textures and represent them on the terrain with a blend map. Each channel of the blend map (RGBA) represents the weight of a texture. 
```glsl  
float back_texture_amount = 1 - (blend_map_texture.r + blend_map_texture.g + blend_map_texture.b + blend_map_texture.a);
color = vec3(.8, .8, .8) * back_texture_amount;
	
vec3 t_color1 = texture(tile_texture1, source.uv).rgb * texture(blend_map, blend_map_position).r;
vec3 t_color2 = texture(tile_texture2, source.uv).rgb * texture(blend_map, blend_map_position).g;
vec3 t_color3 = texture(tile_texture3, source.uv).rgb * texture(blend_map, blend_map_position).b;
vec3 t_color4 = texture(tile_texture4, source.uv).rgb * texture(blend_map, blend_map_position).a;

color = color + t_color1 + t_color2 + t_color3 + t_color4;
```
Here is a semi-hand painted section of the terrain:

![](https://github.com/willardt/3.31/blob/main/ss/terrain3.png?raw=true "")

### Level of Detail

We can increase the level of detail of our vertices by adding more. To do this I created a quadtree structure where the numbers of vertices is multiplied by 4 for each level of detail.

[Quadtree wiki](https://en.wikipedia.org/wiki/Quadtree)

Level 0 detail
![](https://github.com/willardt/3.31/blob/main/ss/terrain5.png?raw=true "")
Level 3 detail (third node in quadtree)
![](https://github.com/willardt/3.31/blob/main/ss/terrain4.png?raw=true "")
Level 0 detail wireframe
![](https://github.com/willardt/3.31/blob/main/ss/terrain6.png?raw=true "")
Level 3 detail wireframe
![](https://github.com/willardt/3.31/blob/main/ss/terrain7.png?raw=true "")

Note that when we increase the level of detail we have no data for the height values between vertices. To find this data I approximate the height by averaging the heights of the old vertices. 

Here is how a tile is divided into 4:

![](https://github.com/willardt/3.31/blob/main/ss/average.png?raw=true "")

The corner vertices retain the same value of the original tile, while the new vertices are an average of their adjacent vertices.
```C++
void TerrainNode::generate_heights(int index) {
	_heights.clear();
	_heights.resize(_parent->_heights.size());

	auto valid = [&](const size_t& i) {
		if(i < 0 || i > _parent->_heights.size() - 1) {
			return false;
		}
		return true;
	};
			
	auto avg = [&](const size_t& i , const size_t& i2) {
		std::vector<size_t> indices = { i, i2};
		int divisor = 0;
		GLfloat value = 0.0f;
		for (const auto& i : indices) {
			if (valid(i)) {
				value += _parent->_heights.at(i);
				++divisor;
			}
		}
			
		return value / (GLfloat)divisor;
	};
			
	auto avg2 = [&](const int& i, const int& i2, const int& i3, const int& i4) {
		std::vector<int> indices = {i, i2, i3, i4};
		int divisor = 0;
		GLfloat value = 0.0f;
		for(const auto& i : indices) {
			if (valid(i)) {
				value += _parent->_heights.at(i);
				++divisor;
			}
		}
			
		return value / (GLfloat)divisor;
	};

	size_t in_i = index;
	size_t out_i = 0;
	int count = 0;
		
	bool even = true;
	while (out_i < (_heights.size())) {
		if (even) {
			_heights.at(out_i) = _parent->_heights.at(in_i);
			++count;
		
			while (count < _root->_width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg(in_i - 1, in_i);
				++count;
		
				++out_i;
				_heights.at(out_i) = _parent->_heights.at(in_i);
				++count;
			}
		
			++in_i;
			in_i += _root->_width / 2;
		}
		else {
			_heights.at(out_i) = avg(in_i - _root->_width - 1, in_i);
			++count;
		
			while (count < _root->_width) {
				++out_i;
				++in_i;
				_heights.at(out_i) = avg2(in_i, in_i - _root->_width - 1, in_i - 1, in_i - _root->_width - 2);
				++count;
		
				++out_i;
				_heights.at(out_i) = avg(in_i - _root->_width - 1, in_i);
				++count;
			}
		
			in_i -= _root->_width / 2;
		}
		
		++out_i;
		count = 0;
		even = !even;
	}
}
```

And the result with textures:

![](https://github.com/willardt/3.31/blob/main/ss/terrain8.png?raw=true "")

### Editing Terrain

The best way to show this off is with a video.

[Editing Demo](https://www.youtube.com/watch?v=cKjI6oR3NwI)

## End Note

Work in progress...  
