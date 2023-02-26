/*
 * Copyright 2008 Tristan Heaven
 * Copyright 2008 Paul Wise
 *
 * "Chromium B.S.U." is free software; you can redistribute
 * it and/or use it and/or modify it under the terms of the
 * "Clarified Artistic License"
 */

#ifndef Image_h
#define Image_h

#ifdef HAVE_CONFIG_H
#include <chromium-bsu-config.h>
#endif

#include "compatibility.h"

#if defined(HAVE_APPLE_OPENGL_FRAMEWORK) || defined(HAVE_OPENGL_GL_H)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

//====================================================================
enum ImageMipMap { IMG_NOMIPMAPS, IMG_SIMPLEMIPMAPS, IMG_BUILDMIPMAPS };
enum ImageBlend { IMG_SOLID, IMG_ALPHA, IMG_BLEND1, IMG_BLEND2, IMG_BLEND3 };

class Image
{
public:

	static GLuint load(const char *filename, ImageMipMap mipmap = IMG_NOMIPMAPS, ImageBlend trans = IMG_ALPHA, GLint wrapst = GL_CLAMP, GLint minfilter = GL_LINEAR, GLint magfilter = GL_LINEAR);

private:
	Image();
};

#endif // Image_h
