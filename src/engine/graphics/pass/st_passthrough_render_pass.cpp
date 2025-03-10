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
	st_render_texture* source_buffer,
	st_swap_chain* swap_chain)
{
	st_graphics_context* context = st_graphics_context::get();

	st_texture_desc target_desc;
	context->get_desc(context->get_backbuffer(swap_chain, 0), &target_desc);

	{
		st_attachment_desc attachment = { target_desc._format, e_st_load_op::clear, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._viewport = { 0.0f, 0.0f, float(target_desc._width), float(target_desc._height), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	for (uint32_t i = 0; i < std::size(_framebuffers); ++i)
	{
		st_target_desc target = { context->get_backbuffer(swap_chain, i), context->get_backbuffer_view(swap_chain, i) };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_framebuffers[i] = context->create_framebuffer(desc);
	}

	// Set up the fullscreen material and state.
	_material = std::make_unique<st_fullscreen_material>(
		source_buffer,
		context->get_backbuffer(swap_chain, 0),
		_vertex_format.get(),
		_pass.get());
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

	_material->bind(context, e_st_render_pass_type::passthrough, params, identity, identity, identity);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
	};
	context->begin_render_pass(_pass.get(), _framebuffers[params->_frame_index].get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffers[params->_frame_index].get());
}
