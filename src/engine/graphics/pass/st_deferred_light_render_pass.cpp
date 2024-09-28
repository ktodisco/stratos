/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_deferred_light_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
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
	st_render_texture* output_buffer,
	st_render_texture* output_depth)
{
	st_graphics_context* context = st_graphics_context::get();

	_constant_buffer = context->create_buffer(1, sizeof(st_deferred_light_cb), e_st_buffer_usage::uniform);
	context->add_constant(_constant_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_light_buffer = context->create_buffer(
		1,
		sizeof(st_sphere_light_data),
		e_st_buffer_usage::storage | e_st_buffer_usage::transfer_dest);
	context->set_buffer_meta(_light_buffer.get(), "type_StructuredBuffer_st_sphere_light");

	_material = std::make_unique<st_deferred_light_material>(
		albedo_buffer,
		normal_buffer,
		third_buffer,
		depth_buffer,
		_constant_buffer.get(),
		_light_buffer.get());

	st_target_desc targets[] =
	{
		{ output_buffer, e_st_load_op::clear, e_st_store_op::store }
	};
	st_target_desc ds_target = { output_depth, e_st_load_op::clear, e_st_store_op::store };
	_pass = context->create_render_pass(
		1,
		targets,
		&ds_target);

	st_pipeline_state_desc deferred_light_state_desc;
	_material->get_pipeline_state(&deferred_light_state_desc);

	deferred_light_state_desc._vertex_format = _vertex_format.get();
	deferred_light_state_desc._render_target_count = 1;
	deferred_light_state_desc._render_target_formats[0] = output_buffer->get_format();
	deferred_light_state_desc._depth_stencil_format = output_depth->get_format();

	_pipeline = context->create_pipeline(deferred_light_state_desc, _pass.get());
}

st_deferred_light_render_pass::~st_deferred_light_render_pass()
{
}

void st_deferred_light_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_deferred_light_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline(_pipeline.get());

	// Set global pass resource tables.
	_material->bind(context, params, identity, identity, identity);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		st_depth_stencil_clear_value { 1.0f, 0 }
	};

	context->begin_render_pass(_pass.get(), clears, std::size(clears));

	st_deferred_light_cb constant_data;
	constant_data._inverse_vp = (params->_view * params->_projection).inverse();
	constant_data._inverse_vp.transpose();
	constant_data._eye = st_vec4f(params->_eye, 0.0f);
	if (st_graphics_context::get()->get_api() == e_st_graphics_api::opengl)
		constant_data._depth_reconstruction = st_vec4f(2.0f, 1.0f, 0.0f, 0.0f);
	else
		constant_data._depth_reconstruction = st_vec4f(1.0f, 0.0f, 0.0f, 0.0f);

	context->update_buffer(_constant_buffer.get(), &constant_data, 0, 1);

	// New light buffer.
	st_sphere_light_data light_data;
	light_data._position_power = st_vec4f(params->_light->_position, params->_light->_power);
	light_data._color_radius = st_vec4f(params->_light->_color, params->_light->_radius);
	context->update_buffer(_light_buffer.get(), &light_data, 0, 1);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get());
}
