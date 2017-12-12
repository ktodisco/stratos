/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_render_context.h>

#include <cstdio>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>

st_render_context::st_render_context(HDC device_context) :
	_device_context(device_context)
{
	// Choose a pixel format.
	PIXELFORMATDESCRIPTOR format_desc =
	{ 
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
 
	// Get the device context's best available pixel format match.
	int format_index = ChoosePixelFormat(_device_context, &format_desc); 
 
	// Make that match the device context's current pixel format.
	SetPixelFormat(_device_context, format_index, &format_desc);

	_gl_context = wglCreateContext(_device_context);

	wglMakeCurrent(_device_context, _gl_context);

	GLint major;
	GLint minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("Using OpenGL %d.%d.\n", major, minor);

	// Init GLEW after creating the OpenGL context so it can locate the functions.
	GLint glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK)
	{
		printf("Failed to initialize GLEW. %s. Exiting.\n", glewGetErrorString(glew_init_result));
		exit(1);
	}
}

st_render_context::~st_render_context()
{
	// Destroy the GL context.
	wglDeleteContext(_gl_context);
}

void st_render_context::acquire()
{
	// Acquire for current thread.
	wglMakeCurrent(_device_context, _gl_context);
}

void st_render_context::release()
{
	// Release from current thread.
	wglMakeCurrent(_device_context, 0);
}
