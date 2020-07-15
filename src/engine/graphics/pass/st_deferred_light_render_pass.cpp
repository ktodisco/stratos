/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_deferred_light_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/geometry/st_vertex_format.h>
#include <graphics/light/st_sphere_light.h>
#include <graphics/material/st_deferred_light_material.h>
#include <graphics/st_buffer.h>
#include <graphics/st_constant_buffer.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_pass.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_resource_table.h>

#include <cmath>

st_deferred_light_render_pass::st_deferred_light_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* third_buffer,
	st_render_texture* depth_buffer,
	st_render_texture* output_buffer,
	st_render_texture* output_depth)
{
	_constant_buffer = std::make_unique<st_constant_buffer>(sizeof(st_deferred_light_cb));
	_constant_buffer->add_constant("type_cb0", st_shader_constant_type_block);

	_light_buffer = std::make_unique<st_buffer>(
		1,
		sizeof(st_sphere_light_data),
		e_st_buffer_usage::storage | e_st_buffer_usage::transfer_dest);
	_light_buffer->set_meta("type_StructuredBuffer_st_sphere_light");
	_resources = std::make_unique<st_resource_table>();
	_resources->add_buffer_resource(_light_buffer.get());

	_material = std::make_unique<st_deferred_light_material>(
		albedo_buffer,
		normal_buffer,
		third_buffer,
		depth_buffer,
		_constant_buffer.get());

	st_render_texture* targets[] = { output_buffer };
	_pass = std::make_unique<st_render_pass>(
		1,
		targets,
		output_depth);

	st_pipeline_state_desc deferred_light_state_desc;
	_material->get_pipeline_state(&deferred_light_state_desc);

	deferred_light_state_desc._vertex_format = _vertex_format.get();
	deferred_light_state_desc._render_target_count = 1;
	deferred_light_state_desc._render_target_formats[0] = output_buffer->get_format();
	deferred_light_state_desc._depth_stencil_format = output_depth->get_format();

	_pipeline_state = std::make_unique<st_pipeline_state>(deferred_light_state_desc, _pass.get());
}

st_deferred_light_render_pass::~st_deferred_light_render_pass()
{
}

void st_deferred_light_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker("st_deferred_light_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_viewport(0, 0, params->_width, params->_height);
	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline_state(_pipeline_state.get());

	// Set global pass resource tables.
	_resources->bind(context);

	_pass->begin(context);

	// Update the light information.
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	st_deferred_light_cb constant_data;
	constant_data._inverse_vp = (params->_view * perspective).inverse();
	constant_data._inverse_vp.transpose();
	constant_data._eye = st_vec4f(params->_eye, 0.0f);
#if defined(ST_GRAPHICS_API_OPENGL)
	constant_data._depth_reconstruction = st_vec4f(2.0f, 1.0f, 0.0f, 0.0f);
#else
	constant_data._depth_reconstruction = st_vec4f(1.0f, 0.0f, 0.0f, 0.0f);
#endif

	_constant_buffer->update(context, &constant_data);

	// New light buffer.
	st_sphere_light_data light_data;
	light_data._position_power = st_vec4f(params->_light->_position, params->_light->_power);
	light_data._color_radius = st_vec4f(params->_light->_color, params->_light->_radius);
	_light_buffer->update(context, &light_data, 1);

	_material->bind(context, params, identity, identity, identity);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);

	_pass->end(context);
}
