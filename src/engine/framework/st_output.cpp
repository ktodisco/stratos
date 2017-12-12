/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_output.h"

#include "st_frame_params.h"

#include "graphics/st_material.h"
#include "graphics/st_program.h"
#include "graphics/st_render_context.h"
#include "graphics/st_render_marker.h"
#include "graphics/st_scene_render_pass.h"
#include "graphics/st_ui_render_pass.h"
#include "math/st_mat4f.h"
#include "math/st_quatf.h"
#include "system/st_window.h"

#include <cassert>
#include <iostream>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

st_output::st_output(const st_window* window, st_render_context* render_context) :
	_window(window), _render_context(render_context)
{
	glViewport(0, 0, _window->get_width(), _window->get_height());
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_DEBUG_OUTPUT);
}

st_output::~st_output()
{
}

void st_output::update(st_frame_params* params)
{
	// Acquire the render context.
	_render_context->acquire();

	// Update viewport in case window was resized:
	uint32_t width = _window->get_width();
	uint32_t height = _window->get_height();
	glViewport(0, 0, width, height);
	params->_width = width;
	params->_height = height;

	// Clear viewport:
	glDepthMask(GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);

	st_scene_render_pass scene_pass;
	scene_pass.render(params);

	st_ui_render_pass ui_pass;
	ui_pass.render(params);

	GLenum error = glGetError();
	assert(error == GL_NONE);

	// Swap the frame buffers and release the context.
	SwapBuffers(_render_context->get_device_context());

	_render_context->release();
}
