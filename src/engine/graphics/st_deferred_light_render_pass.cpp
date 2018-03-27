/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_deferred_light_render_pass.h>

#include <graphics/st_deferred_light_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_framebuffer.h>
#include <graphics/st_geometry.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_vertex_format.h>

st_deferred_light_render_pass::st_deferred_light_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* depth_buffer,
	st_render_texture* output_buffer,
	st_render_texture* output_depth)
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

	_default_material = std::make_unique<st_deferred_light_material>(
		albedo_buffer,
		normal_buffer,
		depth_buffer);

	st_pipeline_state_desc deferred_light_state_desc;
	_default_material->get_pipeline_state(&deferred_light_state_desc);

	deferred_light_state_desc._vertex_format = _vertex_format.get();
	deferred_light_state_desc._render_target_count = 1;
	deferred_light_state_desc._render_target_formats[0] = st_texture_format_r8g8b8a8_unorm;
	deferred_light_state_desc._depth_stencil_format = st_texture_format_d24_unorm_s8_uint;

	_pipeline_state = std::make_unique<st_pipeline_state>(deferred_light_state_desc);

	st_render_texture* targets[] = { output_buffer };
	_framebuffer = std::make_unique<st_framebuffer>(
		1,
		targets,
		output_depth);
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

	context->set_pipeline_state(_pipeline_state.get());

	_framebuffer->bind(context);

	_default_material->bind(context, identity, identity, identity);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);

	context->draw(draw_call);

	_framebuffer->unbind(context);
}
