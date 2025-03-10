/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_deferred_light_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/light/st_directional_light.h>
#include <graphics/light/st_sphere_light.h>
#include <graphics/material/st_deferred_light_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

#include <cmath>

st_deferred_light_render_pass::st_deferred_light_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* third_buffer,
	st_render_texture* depth_buffer,
	st_render_texture* directional_shadow_map,
	st_render_texture* output_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_deferred_light_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_constant_buffer = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _constant_buffer.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_sphere_light_data);
		desc._usage = e_st_buffer_usage::storage | e_st_buffer_usage::transfer_dest;
		_light_buffer = context->create_buffer(desc);
		context->set_buffer_name(_light_buffer.get(), "Light Buffer");
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _light_buffer.get();
		_lbv = context->create_buffer_view(desc);
	}

	{
		st_attachment_desc attachments[] =
		{
			{ output_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._viewport = { 0.0f, 0.0f, float(output_buffer->get_width()), float(output_buffer->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { output_buffer->get_texture(), output_buffer->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_framebuffer = context->create_framebuffer(desc);
	}

	_material = std::make_unique<st_deferred_light_material>(
		albedo_buffer,
		normal_buffer,
		third_buffer,
		depth_buffer,
		directional_shadow_map,
		output_buffer,
		_cbv.get(),
		_lbv.get(),
		_vertex_format.get(),
		_pass.get());

	_cb._sun_shadow_dim = st_vec4f
	{
		static_cast<float>(directional_shadow_map->get_width()),
		static_cast<float>(directional_shadow_map->get_height()),
		1.0f / directional_shadow_map->get_width(),
		1.0f / directional_shadow_map->get_height()
	};
}

st_deferred_light_render_pass::~st_deferred_light_render_pass()
{
	_cbv = nullptr;
	_lbv = nullptr;
	_constant_buffer = nullptr;
	_light_buffer = nullptr;
	_pass = nullptr;
}

void st_deferred_light_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_deferred_light_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);

	_cb._inverse_vp = (params->_view * params->_projection).inverse();
	_cb._eye = st_vec4f(params->_eye, 0.0f);
	if (context->get_api() == e_st_graphics_api::opengl)
		_cb._depth_reconstruction = st_vec4f(2.0f, 1.0f, 0.5f, 0.5f);
	else
		_cb._depth_reconstruction = st_vec4f(1.0f, 0.0f, 0.5f, -0.5f);

	if (params->_sun)
	{
		_cb._sun_direction_power = st_vec4f(params->_sun->_direction, params->_sun->_power);
		_cb._sun_color = st_vec4f(params->_sun->_color, 1.0f);
		_cb._sun_vp = params->_sun_view * params->_sun_projection;
	}

	context->update_buffer(_constant_buffer.get(), &_cb, 0, 1);

	// New light buffer.
	st_sphere_light_data light_data;
	light_data._position_power = st_vec4f(params->_light->_position, params->_light->_power);
	light_data._color_radius = st_vec4f(params->_light->_color, params->_light->_radius);
	context->update_buffer(_light_buffer.get(), &light_data, 0, 1);

	// Set global pass resource tables.
	_material->bind(context, e_st_render_pass_type::deferred, params, identity, identity, identity);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		st_depth_stencil_clear_value { 1.0f, 0 }
	};

	// This must come after bind, because bind is going to issue barriers, which on some
	// platforms cannot happen during a render pass.
	context->begin_render_pass(_pass.get(), _framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}
