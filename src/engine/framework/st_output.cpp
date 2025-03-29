/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_output.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_material.h>
#include <graphics/pass/st_atmosphere_pass.h>
#include <graphics/pass/st_bloom_render_pass.h>
#include <graphics/pass/st_deferred_light_render_pass.h>
#include <graphics/pass/st_directional_shadow_pass.h>
#include <graphics/pass/st_display_pass.h>
#include <graphics/pass/st_gbuffer_render_pass.h>
#include <graphics/pass/st_smaa_pass.h>
#include <graphics/pass/st_tonemap_render_pass.h>
#include <graphics/pass/st_ui_render_pass.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <gui/st_imgui.h>

#include <math/st_mat4f.h>
#include <math/st_quatf.h>
#include <math/st_vec4f.h>

#include <system/st_window.h>

#include <cassert>
#include <iostream>

#include <Windows.h>

st_output* st_output::_this = nullptr;

st_output::st_output(const st_window* window, st_graphics_context* context) :
	_window(window), _width(window->get_width()), _height(window->get_height()), _graphics_context(context)
{
	// Create the swap chain first.
	{
		_backbuffer_format = choose_backbuffer_format();
		_backbuffer_color_space = (_backbuffer_format == st_format_r10g10b10a2_unorm) ?
			st_color_space_st2084 :
			st_color_space_srgb;

		st_swap_chain_desc desc;
		desc._width = _window->get_width();
		desc._height = _window->get_height();
		desc._format = _backbuffer_format;
		desc._color_space = _backbuffer_color_space;
		desc._window_handle = _window->get_window_handle();

		_swap_chain = context->create_swap_chain(desc);
	}

	recreate_textures(_graphics_context);
	recreate_passes(_graphics_context);

	_this = this;
}

st_output::~st_output()
{
	_this = nullptr;
}

bool st_output::update_swap_chain()
{
	bool recreated = false;

	if (_out_of_date ||
		_window->get_width() != _width ||
		_window->get_height() != _height)
	{
		_graphics_context->wait_for_idle();

		{
			_backbuffer_format = choose_backbuffer_format();
			_backbuffer_color_space = (_backbuffer_format == st_format_r10g10b10a2_unorm) ?
				st_color_space_st2084 :
				st_color_space_srgb;

			st_swap_chain_desc desc;
			desc._width = _window->get_width();
			desc._height = _window->get_height();
			desc._format = _backbuffer_format;
			desc._color_space = _backbuffer_color_space;
			desc._window_handle = _window->get_window_handle();

			_graphics_context->reconfigure_swap_chain(desc, _swap_chain.get());
		}

		_graphics_context->acquire();
		_graphics_context->begin_frame();

		recreate_textures(_graphics_context);
		recreate_passes(_graphics_context);

		_graphics_context->end_frame();
		_graphics_context->execute();
		_graphics_context->release();

		_width = _window->get_width();
		_height = _window->get_height();

		recreated = true;
	}

	return recreated;
}

void st_output::update(st_frame_params* params)
{
	_graphics_context->acquire();

	_graphics_context->begin_frame();
	params->_frame_index = _graphics_context->get_frame_index();
	params->_color_space = _backbuffer_color_space;

	_atmosphere_transmission->compute(_graphics_context, params);
	_atmosphere_sky->render(_graphics_context, params);

	_directional_shadow_pass->render(_graphics_context, params);
	_gbuffer_pass->render(_graphics_context, params);
	_deferred_pass->render(_graphics_context, params);
	_atmosphere_pass->render(_graphics_context, params);
	_bloom_pass->render(_graphics_context, params);
	_tonemap_pass->render(_graphics_context, params);
	_smaa_pass->render(_graphics_context, params);
	_ui_pass->render(_graphics_context, params);

	e_st_swap_chain_status status = _graphics_context->acquire_backbuffer(_swap_chain.get());
	if (status != e_st_swap_chain_status::current)
	{
		_out_of_date = true;
		return;
	}

	_graphics_context->transition(
		_graphics_context->get_backbuffer(_swap_chain.get(), params->_frame_index),
		st_texture_state_render_target);

	_display_pass->render(_graphics_context, params);

	// Swap the frame buffers and release the context.
	_graphics_context->transition(
		_graphics_context->get_backbuffer(_swap_chain.get(), params->_frame_index),
		st_texture_state_present);

	_graphics_context->end_frame();
	_graphics_context->present(_swap_chain.get());

	_graphics_context->release();
}

void st_output::get_target_formats(e_st_render_pass_type type, st_graphics_state_desc& desc)
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

e_st_format st_output::choose_backbuffer_format()
{
	std::vector<e_st_format> formats;
	formats.reserve(4);

	_graphics_context->get_supported_formats(_window, formats);

	// Order of preference for backbuffer formats. HDR first, SDR second.
	e_st_format format_order[] = {
		st_format_r10g10b10a2_unorm,
		st_format_r8g8b8a8_unorm,
	};

	for (uint32_t i = 0; i < _countof(format_order); ++i)
	{
		if (std::find(formats.begin(), formats.end(), format_order[i]) != formats.end())
			return format_order[i];
	}

	// As a last resort fallback to standard.
	return st_format_r8g8b8a8_unorm;
}

void st_output::recreate_textures(class st_graphics_context* context)
{
	_transmittance = nullptr;
	_sky_view = nullptr;
	_directional_shadow_map = nullptr;
	_gbuffer_albedo_target = nullptr;
	_gbuffer_normal_target = nullptr;
	_gbuffer_third_target = nullptr;
	_depth_stencil_target = nullptr;
	_deferred_target = nullptr;
	_bloom_target = nullptr;
	_tonemap_target = nullptr;

	_directional_shadow_map = std::make_unique<st_render_texture>(
		context,
		2048,
		2048,
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_depth_stencil_clear_value { 1.0f, 0 },
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
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Gbuffer Normal");
	_gbuffer_third_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Gbuffer Third");
	_depth_stencil_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_depth_stencil_clear_value { 1.0f, 0 },
		"Gbuffer Depth");

	_deferred_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Deferred Target");

	_bloom_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width() / 2,
		_window->get_height() / 2,
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Bloom Target");

	_tonemap_target = std::make_unique<st_render_texture>(
		context,
		_window->get_width(),
		_window->get_height(),
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Tonemap Target");

	_transmittance = std::make_unique<st_render_texture>(
		context,
		64,
		256,
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::storage | e_st_texture_usage::sampled,
		st_texture_state_unordered_access,
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		"Atmosphere Transmittance");

	_sky_view = std::make_unique<st_render_texture>(
		context,
		256,
		256,
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		"Sky View");
}

void st_output::recreate_passes(class st_graphics_context* context)
{
	_atmosphere_transmission = nullptr;
	_atmosphere_sky = nullptr;
	_atmosphere_pass = nullptr;
	_directional_shadow_pass = nullptr;
	_gbuffer_pass = nullptr;
	_deferred_pass = nullptr;
	_bloom_pass = nullptr;
	_tonemap_pass = nullptr;
	_smaa_pass = nullptr;
	_ui_pass = nullptr;
	_display_pass = nullptr;

	_atmosphere_transmission = std::make_unique<st_atmosphere_transmission_pass>(
		_transmittance.get());
	_atmosphere_sky = std::make_unique<st_atmosphere_sky_pass>(
		_transmittance.get(),
		_sky_view.get());
	_atmosphere_pass = std::make_unique<st_atmosphere_render_pass>(
		_deferred_target.get(),
		_depth_stencil_target.get(),
		_transmittance.get(),
		_sky_view.get());

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
	_smaa_pass = std::make_unique<st_smaa_pass>(
		_tonemap_target.get(),
		_depth_stencil_target.get(),
		_deferred_target.get());
	_ui_pass = std::make_unique<st_ui_render_pass>(_window, _deferred_target.get());
	_display_pass = std::make_unique<st_display_pass>(_deferred_target.get(), _swap_chain.get());
}
