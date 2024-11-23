/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_output.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_material.h>
#include <graphics/pass/st_bloom_render_pass.h>
#include <graphics/pass/st_deferred_light_render_pass.h>
#include <graphics/pass/st_directional_shadow_pass.h>
#include <graphics/pass/st_gbuffer_render_pass.h>
#include <graphics/pass/st_passthrough_render_pass.h>
#include <graphics/pass/st_tonemap_render_pass.h>
#include <graphics/pass/st_ui_render_pass.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <math/st_mat4f.h>
#include <math/st_quatf.h>
#include <math/st_vec4f.h>

#include <system/st_window.h>

#include <cassert>
#include <iostream>

#include <Windows.h>

st_output* st_output::_this = nullptr;

st_output::st_output(const st_window* window, st_graphics_context* context) :
	_window(window), _graphics_context(context)
{
	_directional_shadow_map = std::make_unique<st_render_texture>(
		context,
		2048,
		2048,
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 1.0f, (float)(0), 0.0f, 0.0f }),
		"Directional Shadow Map");

	_gbuffer_albedo_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Gbuffer Albedo");
	_gbuffer_normal_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r16g16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Gbuffer Normal");
	_gbuffer_third_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Gbuffer Third");
	_depth_stencil_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 1.0f, (float)(0), 0.0f, 0.0f }),
		"Gbuffer Depth");

	_deferred_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Deferred Target");

	_bloom_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width() / 2,
		_window->get_height() / 2,
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Bloom Target");

	_tonemap_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Tonemap Target");

	_directional_shadow_pass = std::make_unique<st_directional_shadow_pass>(
		_directional_shadow_map.get());
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
		_directional_shadow_map.get(),
		_deferred_target.get());
	_bloom_pass = std::make_unique<st_bloom_render_pass>(
		_deferred_target.get(),
		_bloom_target.get());
	_tonemap_pass = std::make_unique<st_tonemap_render_pass>(
		_deferred_target.get(),
		_bloom_target.get(),
		_tonemap_target.get());
	_passthrough_pass = std::make_unique<st_passthrough_render_pass>(
		_tonemap_target.get());
	_ui_pass = std::make_unique<st_ui_render_pass>();

	_this = this;
}

st_output::~st_output()
{
	_this = nullptr;
}

void st_output::update(st_frame_params* params)
{
	// Acquire the render context.
	_graphics_context->acquire();

	_graphics_context->begin_frame();

	_directional_shadow_pass->render(_graphics_context, params);
	_gbuffer_pass->render(_graphics_context, params);
	_deferred_pass->render(_graphics_context, params);
	_bloom_pass->render(_graphics_context, params);
	_tonemap_pass->render(_graphics_context, params);

	_graphics_context->transition_backbuffer_to_target();

	_passthrough_pass->render(_graphics_context, params);
	_ui_pass->render(_graphics_context, params);

	// Swap the frame buffers and release the context.
	_graphics_context->transition_backbuffer_to_present();
	_graphics_context->end_frame();
	_graphics_context->swap();

	_graphics_context->release();
}

void st_output::get_target_formats(e_st_render_pass_type type, st_pipeline_state_desc& desc)
{
	// TODO: Assert only one bit set in the type argument.
	switch (type)
	{
	case e_st_render_pass_type::shadow: _directional_shadow_pass->get_target_formats(desc); break;
	case e_st_render_pass_type::gbuffer: _gbuffer_pass->get_target_formats(desc); break;
	case e_st_render_pass_type::ui: _ui_pass->get_target_formats(desc); break;
	default:
		assert(false);
	};
}
