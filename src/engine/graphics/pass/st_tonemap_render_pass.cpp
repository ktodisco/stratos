/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_tonemap_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/material/st_tonemap_material.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_tonemap_render_pass::st_tonemap_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	st_target_desc targets[] =
	{
		{ target_buffer, e_st_load_op::clear, e_st_store_op::store }
	};
	_pass = context->create_render_pass(
		1,
		targets,
		nullptr);

	_material = std::make_unique<st_tonemap_material>(
		source_buffer,
		target_buffer,
		_vertex_format.get(),
		_pass.get());
}

st_tonemap_render_pass::~st_tonemap_render_pass()
{
}

void st_tonemap_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_tonemap_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);

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
