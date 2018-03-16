/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_fullscreen_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/st_fullscreen_material.h>
#include <graphics/st_geometry.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_vertex_format.h>

#include <cstdint>

st_fullscreen_render_pass::st_fullscreen_render_pass(
	st_render_texture* input)
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->finalize();

	const float verts[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
	};

	const uint16_t indices[] =
	{
		2, 1, 0,
		3, 2, 0,
	};

	_fullscreen_quad = std::make_unique<st_geometry>(
		_vertex_format.get(),
		(void*)verts,
		static_cast<uint32_t>(sizeof(float) * 3),
		12,
		(uint16_t*)indices,
		6);

	// Set up the fullscreen material and state.
	_default_material = std::make_unique<st_fullscreen_material>(input);

	st_pipeline_state_desc fullscreen_state_desc;
	_default_material->get_pipeline_state(&fullscreen_state_desc);

	fullscreen_state_desc._vertex_format = _vertex_format.get();
	fullscreen_state_desc._render_target_count = 1;
	fullscreen_state_desc._render_target_formats[0] = st_texture_format_r8g8b8a8_unorm;
	fullscreen_state_desc._depth_stencil_format = st_texture_format_d24_unorm_s8_uint;

	_pipeline_state = std::make_unique<st_pipeline_state>(fullscreen_state_desc);
}

st_fullscreen_render_pass::~st_fullscreen_render_pass()
{
}

void st_fullscreen_render_pass::render(st_render_context* context, const st_frame_params* params)
{
	st_render_marker marker("ga_fullscreen_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline_state(_pipeline_state.get());

	_default_material->bind(context, identity, identity, identity);

	st_static_drawcall draw_call;
	draw_call._name = "st_model_component";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);
}
