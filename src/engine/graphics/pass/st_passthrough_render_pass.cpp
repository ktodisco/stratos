/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_passthrough_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_fullscreen_material.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_passthrough_render_pass::st_passthrough_render_pass(
	st_render_texture* source_buffer)
{
	st_render_texture* targets[] =
	{
		st_render_context::get()->get_present_target()
	};
	_pass = std::make_unique<st_render_pass>(
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

	_pipeline_state = std::make_unique<st_pipeline_state>(fullscreen_state_desc, _pass.get());
}

st_passthrough_render_pass::~st_passthrough_render_pass()
{
}

void st_passthrough_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker("st_passthrough_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_viewport(0, 0, params->_width, params->_height);
	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline_state(_pipeline_state.get());

	_material->bind(context, params, identity, identity, identity);

	st_vec4f clears[] =
	{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	};

	_pass->begin(context, clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);

	_pass->end(context);
}
