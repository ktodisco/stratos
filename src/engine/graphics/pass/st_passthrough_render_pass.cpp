/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_passthrough_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/material/st_fullscreen_material.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_passthrough_render_pass::st_passthrough_render_pass(
	st_render_texture* source_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	st_target_desc targets[] =
	{
		{ st_graphics_context::get()->get_present_target(), e_st_load_op::clear, e_st_store_op::store }
	};
	_pass = context->create_render_pass(
		1,
		targets,
		nullptr);

	// Set up the fullscreen material and state.
	_material = std::make_unique<st_fullscreen_material>(source_buffer);

	st_pipeline_state_desc fullscreen_state_desc;
	_material->get_pipeline_state(&fullscreen_state_desc);

	fullscreen_state_desc._vertex_format = _vertex_format.get();
	fullscreen_state_desc._render_target_count = 1;
	fullscreen_state_desc._render_target_formats[0] = source_buffer->get_format();

	_pipeline = context->create_pipeline(fullscreen_state_desc, _pass.get());
}

st_passthrough_render_pass::~st_passthrough_render_pass()
{
}

void st_passthrough_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_passthrough_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline(_pipeline.get());

	_material->bind(context, params, identity, identity, identity);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
	};
	context->begin_render_pass(_pass.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get());
}
