/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_tonemap_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_tonemap_material.h>
#include <graphics/st_framebuffer.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_tonemap_render_pass::st_tonemap_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	_material = std::make_unique<st_tonemap_material>(source_buffer);

	st_pipeline_state_desc tonemap_state_desc;
	_material->get_pipeline_state(&tonemap_state_desc);

	tonemap_state_desc._vertex_format = _vertex_format.get();
	tonemap_state_desc._render_target_count = 1;
	tonemap_state_desc._render_target_formats[0] = target_buffer->get_format();

	_pipeline_state = std::make_unique<st_pipeline_state>(tonemap_state_desc);

	st_render_texture* targets[] = { target_buffer };
	_framebuffer = std::make_unique<st_framebuffer>(
		1,
		targets,
		nullptr);
}

st_tonemap_render_pass::~st_tonemap_render_pass()
{
}

void st_tonemap_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker("st_tonemap_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline_state(_pipeline_state.get());

	_framebuffer->bind(context);

	_material->bind(context, identity, identity, identity);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);

	_framebuffer->unbind(context);
}
