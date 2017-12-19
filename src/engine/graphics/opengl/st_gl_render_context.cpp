/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_render_context.h>

#include <graphics/st_drawcall.h>

#include <cstdio>

st_gl_render_context::st_gl_render_context(HDC device_context) :
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

	// TODO: Wrap this in a debug define.
	glEnable(GL_DEBUG_OUTPUT);
}

st_gl_render_context::~st_gl_render_context()
{
	// Destroy the GL context.
	wglDeleteContext(_gl_context);
}

void st_gl_render_context::acquire()
{
	// Acquire for current thread.
	wglMakeCurrent(_device_context, _gl_context);
}

void st_gl_render_context::release()
{
	// Release from current thread.
	wglMakeCurrent(_device_context, 0);
}

void st_gl_render_context::set_viewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}


void st_gl_render_context::set_depth_state(bool enable, GLenum func)
{
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	glDepthFunc(func);
}

void st_gl_render_context::set_cull_state(bool enable)
{
	if (enable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);
}

void st_gl_render_context::set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor)
{
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);

	glBlendFunc(src_factor, dst_factor);
}

void st_gl_render_context::set_depth_mask(bool enable)
{
	glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void st_gl_render_context::set_clear_color(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;
}

void st_gl_render_context::clear(unsigned int clear_flags)
{
	GLbitfield flags = 0;
	flags |= (clear_flags & st_clear_flag_color) ? GL_COLOR_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_depth) ? GL_DEPTH_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_stencil) ? GL_STENCIL_BUFFER_BIT : 0;

	glClear(flags);
}

void st_gl_render_context::draw(const st_static_drawcall& drawcall)
{
	glBindVertexArray(drawcall._vao);
	glDrawElements(drawcall._draw_mode, drawcall._index_count, GL_UNSIGNED_SHORT, 0);
}

void st_gl_render_context::draw(const st_dynamic_drawcall& drawcall)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint pos;
	glGenBuffers(1, &pos);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec3f) * drawcall._positions.size(), &drawcall._positions[0], GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLuint texcoord;
	if (!drawcall._texcoords.empty())
	{
		glGenBuffers(1, &texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec2f) * drawcall._texcoords.size(), &drawcall._texcoords[0], GL_STREAM_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

	GLuint indices;
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * drawcall._indices.size(), &drawcall._indices[0], GL_STREAM_DRAW);

	glDrawElements(drawcall._draw_mode, (GLsizei)drawcall._indices.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &indices);
	if (!drawcall._texcoords.empty())
	{
		glDeleteBuffers(1, &texcoord);
	}
	glDeleteBuffers(1, &pos);
	glDeleteVertexArrays(1, &vao);
	glBindVertexArray(0);
}

void st_gl_render_context::swap()
{
	SwapBuffers(_device_context);
}
