/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_output.h"

#include "st_frame_params.h"

#include "graphics/st_material.h"
#include "graphics/st_program.h"
#include "graphics/st_render_marker.h"
#include "graphics/st_scene_render_pass.h"
#include "graphics/st_ui_render_pass.h"
#include "math/st_mat4f.h"
#include "math/st_quatf.h"

#include <cassert>
#include <iostream>
#include <SDL.h>

#include <windows.h>

#define GLEW_STATIC
#include <GL/glew.h>

st_output::st_output(void* win) : _window(win)
{
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_DEBUG_OUTPUT);
}

st_output::~st_output()
{
}

void st_output::update(st_frame_params* params)
{
	// Update viewport in case window was resized:
	int width, height;
	SDL_GetWindowSize(static_cast<SDL_Window* >(_window), &width, &height);
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

	// Swap frame buffers:
	SDL_GL_SwapWindow(static_cast<SDL_Window* >(_window));
}
