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
	st_render_texture* bloom_buffer,
	st_render_texture* target_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_attachment_desc attachment = { target_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._viewport = { 0.0f, 0.0f, float(target_buffer->get_width()), float(target_buffer->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { target_buffer->get_texture(), target_buffer->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_framebuffer = context->create_framebuffer(desc);
	}

	_material = std::make_unique<st_tonemap_material>(
		source_buffer,
		bloom_buffer,
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

	_material->bind(context, e_st_render_pass_type::tonemap, params, identity, identity, identity);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
	};

	context->begin_render_pass(_pass.get(), _framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}
