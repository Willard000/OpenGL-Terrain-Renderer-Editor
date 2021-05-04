# OpenGL 3D Terrain Editor

Terrain renderer and editor.  

# Table of Contencts

[Libraries](#libraries)  
[How it works](#how-it-works)
  - [Terrain Mesh](#image-class)
  - [Reading Images](#reading-images)
  - [Compression](#compression)
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
