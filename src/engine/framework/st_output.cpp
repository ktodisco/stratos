/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_output.h"

#include "st_frame_params.h"

#include "graphics/st_material.h"
#include "graphics/st_render_context.h"
#include "graphics/st_render_marker.h"
#include "graphics/st_scene_render_pass.h"
#include "graphics/st_ui_render_pass.h"
#include "math/st_mat4f.h"
#include "math/st_quatf.h"
#include "system/st_window.h"

#include <cassert>
#include <iostream>

#include <Windows.h>

st_output::st_output(const st_window* window, st_render_context* render_context) :
	_window(window), _render_context(render_context)
{
	_render_context->set_viewport(0, 0, _window->get_width(), _window->get_height());
	_render_context->set_scissor(0, 0, window->get_width(), window->get_height());
	_render_context->set_depth_state(true, k_st_depth_less);

	_scene_pass = std::make_unique<st_scene_render_pass>();
	_ui_pass = std::make_unique<st_ui_render_pass>();
}

st_output::~st_output()
{
}

void st_output::update(st_frame_params* params)
{
	// Acquire the render context.
	_render_context->acquire();

	_render_context->begin_frame();

	// Update viewport in case window was resized.
	uint32_t width = _window->get_width();
	uint32_t height = _window->get_height();
	_render_context->set_viewport(0, 0, width, height);
	_render_context->set_scissor(0, 0, width, height);
	params->_width = width;
	params->_height = height;

	_render_context->transition_backbuffer_to_target();

	// Clear viewport.
	_render_context->set_depth_mask(true);
	_render_context->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	_render_context->clear(st_clear_flag_color | st_clear_flag_depth);

	_render_context->set_cull_state(true);

	_scene_pass->render(_render_context, params);

	_ui_pass->render(_render_context, params);

	// Swap the frame buffers and release the context.
	_render_context->transition_backbuffer_to_present();
	_render_context->end_frame();
	_render_context->swap();

	_render_context->release();
}
