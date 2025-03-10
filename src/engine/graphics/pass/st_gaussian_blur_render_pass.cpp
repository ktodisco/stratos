/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_gaussian_blur_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/material/st_gaussian_blur_material.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_gaussian_blur_render_pass::st_gaussian_blur_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	// Set up the intermediate render target between the two blur passes.
	_intermediate_target = std::make_unique<st_render_texture>(
		context,
		source_buffer->get_width(),
		source_buffer->get_height(),
		source_buffer->get_format(),
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Gaussian Blur Intermediate");

	{
		st_attachment_desc attachments[] =
		{
			{ _intermediate_target->get_format(), e_st_load_op::clear, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._viewport = { 0.0f, 0.0f, float(_intermediate_target->get_width()), float(_intermediate_target->get_height()), 0.0f, 1.0f };

		_vertical_blur_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { _intermediate_target->get_texture(), _intermediate_target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _vertical_blur_pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_vertical_blur_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_attachment_desc attachments[] =
		{
			{ target_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._viewport = { 0.0f, 0.0f, float(target_buffer->get_width()), float(target_buffer->get_height()), 0.0f, 1.0f };

		_horizontal_blur_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { target_buffer->get_texture(), target_buffer->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _horizontal_blur_pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_horizontal_blur_framebuffer = context->create_framebuffer(desc);
	}

	_vertical_blur_material = std::make_unique<st_gaussian_blur_vertical_material>(
		source_buffer,
		_intermediate_target.get(),
		_vertex_format.get(),
		_vertical_blur_pass.get());
	_horizontal_blur_material = std::make_unique<st_gaussian_blur_horizontal_material>(
		_intermediate_target.get(),
		target_buffer,
		_vertex_format.get(),
		_horizontal_blur_pass.get());
}

st_gaussian_blur_render_pass::~st_gaussian_blur_render_pass()
{
}

void st_gaussian_blur_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_gaussian_blur_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, _intermediate_target->get_width(), _intermediate_target->get_height());

	{
		st_render_marker marker(context, "Vertical Blur");

		_vertical_blur_material->bind(context, e_st_render_pass_type::gaussian, params, identity, identity, identity);

		st_clear_value clears[] =
		{
			st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		};
		context->begin_render_pass(_vertical_blur_pass.get(), _vertical_blur_framebuffer.get(), clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);
		draw_call._draw_mode = st_primitive_topology_triangles;

		context->draw(draw_call);

		context->end_render_pass(_vertical_blur_pass.get(), _vertical_blur_framebuffer.get());
	}

	{
		st_render_marker marker(context, "Horizontal Blur");

		_horizontal_blur_material->bind(context, e_st_render_pass_type::gaussian, params, identity, identity, identity);

		st_clear_value clears[] =
		{
			st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		};
		context->begin_render_pass(_horizontal_blur_pass.get(), _horizontal_blur_framebuffer.get(), clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);
		draw_call._draw_mode = st_primitive_topology_triangles;

		context->draw(draw_call);

		context->end_render_pass(_horizontal_blur_pass.get(), _horizontal_blur_framebuffer.get());
	}
}
