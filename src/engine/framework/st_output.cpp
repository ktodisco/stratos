/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_output.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>

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
#include <graphics/st_shader_manager.h>

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
	_this = this;

	// Create the device and graphics command structures.
	st_device_desc device_desc;
	_device = context->create_device(device_desc);

	st_command_queue_desc cq_desc;
	_command_queue = _device->create_command_queue(cq_desc);

	st_fence_desc fence_desc;
	_fence = _device->create_fence(fence_desc);

	for (int f = 0; f < k_max_frames; ++f)
	{
		st_command_allocator_desc ca_desc;
		_command_allocators[f] = _device->create_command_allocator(ca_desc);

		st_command_list_desc cl_desc;
		cl_desc.allocator = _command_allocators[f].get();
		_command_lists[f] = _device->create_command_list(cl_desc);

		st_command_allocator_desc upload_alloc_desc;
		_upload_command_allocators[f] = _device->create_command_allocator(upload_alloc_desc);

		st_command_list_desc upload_desc;
		upload_desc.allocator = _upload_command_allocators[f].get();
		_upload_command_lists[f] = _device->create_command_list(upload_desc);
	}

	// Create the shader manager, loading all the shaders.
	_shader_manager = std::make_unique<st_shader_manager>(_device.get());

	// Create resources shared by many systems of the application.
	create_global_resources(_device.get());

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
		desc._command_queue = _command_queue.get();

		_swap_chain = _device->create_swap_chain(desc);
	}

	_frame_index = _device->get_backbuffer_index(_swap_chain.get());

	// Immediately begin recording an upload command list.
	_upload_command_allocators[_frame_index]->reset();
	_upload_command_lists[_frame_index]->begin(_upload_command_allocators[_frame_index].get());

	recreate_textures();
	recreate_passes();
}

st_output::~st_output()
{
	destroy_passes();
	destroy_textures();
	destroy_global_resources();

	_fence = nullptr;
	_swap_chain = nullptr;

	for (int f = 0; f < k_max_frames; ++f)
	{
		_command_lists[f] = nullptr;
		_command_allocators[f] = nullptr;

		_upload_command_lists[f] = nullptr;
		_upload_command_allocators[f] = nullptr;
	}
	_command_queue = nullptr;

	_shader_manager = nullptr;

	_device = nullptr;

	_this = nullptr;
}

bool st_output::update_swap_chain()
{
	bool recreated = false;

	if (_out_of_date ||
		_window->get_width() != _width ||
		_window->get_height() != _height)
	{
		_device->wait(_fence.get(), _frame_counter - 1);

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

			_device->reconfigure_swap_chain(desc, _swap_chain.get());
		}

		recreate_textures();
		recreate_passes();

		_upload_command_lists[_frame_index]->end();
		_command_queue->execute(_upload_command_lists[_frame_index].get());
		_command_queue->signal(_fence.get(), _frame_counter);
		_device->wait(_fence.get(), _frame_counter);
		_frame_counter++;

		_frame_index = _device->get_backbuffer_index(_swap_chain.get());

		_upload_command_allocators[_frame_index]->reset();
		_upload_command_lists[_frame_index]->begin(_upload_command_allocators[_frame_index].get());

		_width = _window->get_width();
		_height = _window->get_height();

		recreated = true;
	}

	return recreated;
}

void st_output::update(st_frame_params* params)
{
	st_command_allocator* command_allocator = _command_allocators[_frame_index].get();
	st_command_list* command_list = _command_lists[_frame_index].get();

	command_allocator->reset();
	command_list->begin(command_allocator);

	params->_frame_index = _frame_index;
	params->_color_space = _backbuffer_color_space;

	_atmosphere_transmission->compute(command_list, params);
	_atmosphere_sky->render(command_list, params);

	_directional_shadow_pass->render(command_list, params);
	_gbuffer_pass->render(command_list, params);
	_deferred_pass->render(command_list, params);
	_atmosphere_pass->render(command_list, params);
	_bloom_pass->render(command_list, params);
	_tonemap_pass->render(command_list, params);
	_smaa_pass->render(command_list, params);
	_ui_pass->render(command_list, params);

	e_st_swap_chain_status status = _device->acquire_backbuffer(_swap_chain.get());
	if (status == e_st_swap_chain_status::current)
	{
		_out_of_date = false;

		command_list->transition(
			_device->get_backbuffer(_swap_chain.get(), params->_frame_index),
			st_texture_state_render_target);

		_display_pass->render(command_list, params);

		// Swap the frame buffers and release the context.
		command_list->transition(
			_device->get_backbuffer(_swap_chain.get(), params->_frame_index),
			st_texture_state_present);
	}
	else
	{
		_out_of_date = true;
	}

	command_list->end();

	_upload_command_lists[_frame_index]->end();
	_command_queue->execute(_upload_command_lists[_frame_index].get());
	_command_queue->execute(command_list);

	if (!_out_of_date)
		_command_queue->present(_swap_chain.get());

	_command_queue->signal(_fence.get(), _frame_counter);

	if (!_out_of_date)
		_frame_index = _device->get_backbuffer_index(_swap_chain.get());

	// TODO: Better parallelization.
	// What actually needs to happen here is signal is called with a _frame_end fence,
	// and then we provide that fence whenever we call wait_for_idle elsewhere.
	_device->wait(_fence.get(), _frame_counter);

	_frame_counter++;

	// Already begin recording the next upload commands.
	_upload_command_allocators[_frame_index]->reset();
	_upload_command_lists[_frame_index]->begin(_upload_command_allocators[_frame_index].get());
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

void st_output::destroy_textures()
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
}

void st_output::destroy_passes()
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
}

void st_output::recreate_textures()
{
	destroy_textures();

	_directional_shadow_map = std::make_unique<st_render_texture>(
		_device.get(),
		2048,
		2048,
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_depth_stencil_clear_value { 1.0f, 0 },
		"Directional Shadow Map");

	_gbuffer_albedo_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Gbuffer Albedo");
	_gbuffer_normal_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_r16g16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Gbuffer Normal");
	_gbuffer_third_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Gbuffer Third");
	_depth_stencil_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_d24_unorm_s8_uint,
		e_st_texture_usage::depth_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_depth_stencil_clear_value { 1.0f, 0 },
		"Gbuffer Depth");

	_deferred_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Deferred Target");

	_bloom_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width() / 2,
		_window->get_height() / 2,
		st_format_r16g16b16a16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Bloom Target");

	_tonemap_target = std::make_unique<st_render_texture>(
		_device.get(),
		_window->get_width(),
		_window->get_height(),
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 1.0f }),
		"Tonemap Target");

	_transmittance = std::make_unique<st_render_texture>(
		_device.get(),
		64,
		256,
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::storage | e_st_texture_usage::sampled,
		st_texture_state_unordered_access,
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		"Atmosphere Transmittance");

	_sky_view = std::make_unique<st_render_texture>(
		_device.get(),
		192,
		104,
		st_format_r11g11b10_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		"Sky View");
}

void st_output::recreate_passes()
{
	destroy_passes();

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
