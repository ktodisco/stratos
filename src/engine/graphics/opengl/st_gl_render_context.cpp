/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_render_context.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/opengl/st_gl_drawcall.h>
#include <graphics/opengl/st_gl_pipeline_state.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader.h>
#include <system/st_window.h>

#include <cstdio>

st_gl_render_context::st_gl_render_context(const st_window* window)
{
	_device_context = GetDC(window->get_window_handle());

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

	// Init GLEW after creating the OpenGL context so it can locate the functions.
	GLint glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK)
	{
		printf("Failed to initialize GLEW. %s. Exiting.\n", glewGetErrorString(glew_init_result));
		exit(1);
	}
	
	const GLint attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 4,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

    _gl_context = wglCreateContextAttribsARB(_device_context, 0, attribs);

	wglMakeCurrent(_device_context, _gl_context);

	GLint major;
	GLint minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("Using OpenGL %d.%d. Version: %s\n", major, minor, (char*)glGetString(GL_VERSION));

#if defined(_DEBUG)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)gl_message_callback, 0);
#endif
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

void st_gl_render_context::set_pipeline_state(const st_gl_pipeline_state* state)
{
	const st_pipeline_state_desc& state_desc = state->get_state();
	state_desc._shader->use();
	_bound_shader = state_desc._shader;

	set_depth_state(
		state_desc._depth_stencil_desc._depth_enable,
		state_desc._depth_stencil_desc._depth_compare);

	set_depth_mask(
		state_desc._depth_stencil_desc._depth_mask != st_depth_write_mask_zero);

	set_blend_state(
		state_desc._blend_desc._target_blend[0]._blend,
		state_desc._blend_desc._target_blend[0]._src_blend,
		state_desc._blend_desc._target_blend[0]._dst_blend);

	set_cull_state(
		state_desc._rasterizer_desc._cull_mode != GL_NONE,
		state_desc._rasterizer_desc._cull_mode);
}

void st_gl_render_context::set_viewport(int x, int y, int width, int height)
{
	glViewport(x, y, width, height);
}

void st_gl_render_context::set_scissor(int left, int top, int right, int bottom)
{
	glScissor(left, top, right - left, bottom - top);
}

void st_gl_render_context::set_depth_state(bool enable, GLenum func)
{
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	glDepthFunc(func);
}

void st_gl_render_context::set_cull_state(bool enable, GLenum mode)
{
	if (enable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glCullFace(mode);
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

	glClearColor(r, g, b, a);
}

void st_gl_render_context::clear(unsigned int clear_flags)
{
	GLbitfield flags = 0;
	flags |= (clear_flags & st_clear_flag_color) ? GL_COLOR_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_depth) ? GL_DEPTH_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_stencil) ? GL_STENCIL_BUFFER_BIT : 0;

	glClear(flags);
}

void st_gl_render_context::draw(const st_gl_static_drawcall& drawcall)
{
	glBindVertexArray(drawcall._vao);
	glDrawElements(drawcall._draw_mode, drawcall._index_count, GL_UNSIGNED_SHORT, 0);
}

void st_gl_render_context::draw(const st_gl_dynamic_drawcall& drawcall)
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

void st_gl_render_context::transition_backbuffer_to_target()
{
}

void st_gl_render_context::transition_backbuffer_to_present()
{
}

void st_gl_render_context::begin_frame()
{
}

void st_gl_render_context::end_frame()
{
}

void st_gl_render_context::swap()
{
	SwapBuffers(_device_context);
}

#endif
