/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_gbuffer_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

#include <cassert>

st_gbuffer_render_pass::st_gbuffer_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* third_buffer,
	st_render_texture* depth_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	// Cache the output formats to use in pipeline state setup.
	_formats[0] = albedo_buffer->get_format();
	_formats[1] = normal_buffer->get_format();
	_formats[2] = third_buffer->get_format();
	_formats[3] = depth_buffer->get_format();

	{
		st_attachment_desc attachments[] =
		{
			{ albedo_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store },
			{ normal_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store },
			{ third_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store },
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._depth_attachment = { depth_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store };
		desc._viewport = { 0.0f, 0.0f, float(albedo_buffer->get_width()), float(albedo_buffer->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc targets[] =
		{
			{ albedo_buffer->get_texture(), albedo_buffer->get_target_view(), },
			{ normal_buffer->get_texture(), normal_buffer->get_target_view(), },
			{ third_buffer->get_texture(), third_buffer->get_target_view() }
		};
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = targets;
		desc._target_count = std::size(targets);
		desc._depth_target = { depth_buffer->get_texture(), depth_buffer->get_target_view() };

		_framebuffer = context->create_framebuffer(desc);
	}
}

st_gbuffer_render_pass::~st_gbuffer_render_pass()
{
}

void st_gbuffer_render_pass::render(st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, "st_gbuffer_render_pass::render");

	context->set_scissor(0, 0, params->_width, params->_height);

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		st_depth_stencil_clear_value { 1.0f, 0 }
	};

	context->begin_render_pass(_pass.get(), _framebuffer.get(), clears, std::size(clears));

	// Clear viewport.
	context->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	context->clear(st_clear_flag_color | st_clear_flag_depth);

	// Draw all static geometry.
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(context, d._name.c_str());

		if (!d._material)
		{
			assert(false);
		}
		else if (d._material->supports_pass(e_st_render_pass_type::gbuffer))
		{
			d._material->bind(context, e_st_render_pass_type::gbuffer, params, params->_projection, params->_view, d._transform);
		}

		context->draw(d);
	}

	context->end_render_pass(_pass.get(), _framebuffer.get());
}

void st_gbuffer_render_pass::get_target_formats(struct st_graphics_state_desc& desc)
{
	desc._pass = _pass.get();
	desc._render_target_count = 3;
	desc._render_target_formats[0] = _formats[0];
	desc._render_target_formats[1] = _formats[1];
	desc._render_target_formats[2] = _formats[2];
	desc._depth_stencil_format = _formats[3];
}
