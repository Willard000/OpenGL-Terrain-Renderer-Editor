#ifndef TEXTURE_H
#define TEXTURE_H

typedef unsigned int GLuint;

struct Texture {
	int			_key		=		-1;
	GLuint		_id			=		 0;
	const char* _type		=		"";
};

#endif