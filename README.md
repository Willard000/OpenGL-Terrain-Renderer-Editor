# OpenGL 3D Terrain Editor

Terrain renderer and editor.  

# Table of Contencts

[Libraries](#libraries)  
[How it works](#how-it-works)
  - [Terrain Mesh](#terrain-mesh)
  - [Height Map](#height-map)
  - [Normal Map](#normal-map)
  - [Filtering](#filtering)
  - [Conversion](#conversion)
  - [Result](#result)

[End Note](#end-note)

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
Here is the result of a flat terrain with simple shading.

![](https://github.com/willardt/3.31/blob/main/ss/terrain.png?raw=true "")  

### Height Map

The next step is to add height to the terrain mesh. This is done by creating an array of height values for each vertex on the mesh.
We dont need 6 height values per tile, we only need 4 since 2 vertices overlap. The size of the height array is calculated by (width + 1) * (length + 1).
The heights are maped per vertex row not per tile since there is overlap between vertices and tiles.

![](https://github.com/willardt/3.31/blob/main/ss/heights.png?raw=true "")

### Normal Map

Normals are stored the same way as the height map. To calculated the normal we first calculate the face normal for each triangle then averaging the normals from all adjacent triangles to a vertex.

We pass the height and normal map as a texture buffer to glsl and acess the height and normal of any vertex with a function like this.

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


Here is the result after adding a height map and normal map.

![](https://github.com/willardt/3.31/blob/main/ss/terrain2.png?raw=true "")

