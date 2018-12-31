/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_output.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_material.h>
#include <graphics/pass/st_deferred_light_render_pass.h>
#include <graphics/pass/st_gbuffer_render_pass.h>
#include <graphics/pass/st_passthrough_render_pass.h>
#include <graphics/pass/st_tonemap_render_pass.h>
#include <graphics/pass/st_ui_render_pass.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

#include <math/st_mat4f.h>
#include <math/st_quatf.h>
#include <math/st_vec4f.h>

#include <system/st_window.h>

#include <cassert>
#include <iostream>

#include <Windows.h>

st_output::st_output(const st_window* window, st_render_context* render_context) :
	_window(window), _render_context(render_context)
{
	_gbuffer_albedo_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_r8g8b8a8_unorm,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
	_gbuffer_albedo_target->set_name("Gbuffer Albedo");
	_gbuffer_normal_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_r32g32b32a32_float,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
	_gbuffer_normal_target->set_name("Gbuffer Normal");
	_gbuffer_third_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_r16g16b16a16_float,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }));
	_depth_stencil_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_d24_unorm_s8_uint,
		st_vec4f({ 1.0f, (float)(0), 0.0f, 0.0f }));
	_depth_stencil_target->set_name("Gbuffer Depth");

	_deferred_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_r16g16b16a16_float,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }));
	_deferred_target->set_name("Deferred Target");
	_deferred_depth = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_d24_unorm_s8_uint,
		st_vec4f({ 1.0f, (float)(0), 0.0f, 0.0f }));

	_tonemap_target = std::make_unique<st_render_texture>(
		_window->get_width(),
		_window->get_height(),
		st_texture_format_r8g8b8a8_unorm,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }));
	_tonemap_target->set_name("Tonemap Target");

	_gbuffer_pass = std::make_unique<st_gbuffer_render_pass>(
		_gbuffer_albedo_target.get(),
		_gbuffer_normal_target.get(),
		_gbuffer_third_target.get(),
		_depth_stencil_target.get());
	_deferred_pass = std::make_unique<st_deferred_light_render_pass>(
		_gbuffer_albedo_target.get(),
		_gbuffer_normal_target.get(),
		_gbuffer_third_target.get(),
		_depth_stencil_target.get(),
		_deferred_target.get(),
		_deferred_depth.get());
	_tonemap_pass = std::make_unique<st_tonemap_render_pass>(
		_deferred_target.get(),
		_tonemap_target.get());
	_passthrough_pass = std::make_unique<st_passthrough_render_pass>(
		_tonemap_target.get());
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

	_gbuffer_pass->render(_render_context, params);
	_deferred_pass->render(_render_context, params);
	_tonemap_pass->render(_render_context, params);

	_render_context->transition_backbuffer_to_target();

	_passthrough_pass->render(_render_context, params);
	_ui_pass->render(_render_context, params);

	// Swap the frame buffers and release the context.
	_render_context->transition_backbuffer_to_present();
	_render_context->end_frame();
	_render_context->swap();

	_render_context->release();
}
