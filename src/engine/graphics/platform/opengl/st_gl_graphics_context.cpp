/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics_context.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/platform/opengl/st_gl_conversion.h>
#include <graphics/platform/opengl/st_gl_device.h>
#include <graphics/platform/opengl/st_gl_shader.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_texture.h>

#include <system/st_window.h>

#include <cassert>
#include <cstdio>

st_gl_graphics_context::st_gl_graphics_context(const st_window* window)
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

	// Turn on vsync.
	// TODO: Test for the extension and have a backup.
	wglSwapIntervalEXT(1);

	_this = this;
}

st_gl_graphics_context::~st_gl_graphics_context()
{
	// Destroy the GL context.
	wglDeleteContext(_gl_context);
}

std::unique_ptr<st_device> st_gl_graphics_context::create_device(const st_device_desc& desc)
{
	std::unique_ptr<st_gl_device> device = std::make_unique<st_gl_device>(_device_context);
	return std::move(device);
}

void st_gl_graphics_context::get_supported_formats(
	const st_window* window,
	st_device* device,
	std::vector<e_st_format>& formats)
{
	formats.push_back(st_format_r8g8b8a8_unorm);
	formats.push_back(st_format_r8g8b8a8_unorm_srgb);
}

#endif
