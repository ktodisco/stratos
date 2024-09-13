/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_bloom_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_bloom_material.h>
#include <graphics/pass/st_gaussian_blur_render_pass.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_bloom_render_pass::st_bloom_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	st_render_context* context = st_render_context::get();

	std::unique_ptr<st_render_texture> half_target = std::make_unique<st_render_texture>(
		context,
		source_buffer->get_width() / 2,
		source_buffer->get_height() / 2,
		source_buffer->get_format(),
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Bloom Half Res");

	std::unique_ptr<st_render_texture> blur_target = std::make_unique<st_render_texture>(
		context,
		source_buffer->get_width() / 2,
		source_buffer->get_height() / 2,
		source_buffer->get_format(),
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }),
		"Bloom Blur Target");

	st_render_texture* targets[] = { half_target.get() };
	_pass = context->create_render_pass(
		1,
		targets,
		nullptr);

	_material = std::make_unique<st_bloom_material>(source_buffer->get_texture());

	st_pipeline_state_desc bloom_state_desc;
	_material->get_pipeline_state(&bloom_state_desc);

	bloom_state_desc._vertex_format = _vertex_format.get();
	bloom_state_desc._render_target_count = 1;
	bloom_state_desc._render_target_formats[0] = target_buffer->get_format();

	_pipeline = context->create_pipeline(bloom_state_desc, _pass.get());

	// Set up the separable blur pass.
	_blur_pass = std::make_unique<st_gaussian_blur_render_pass>(
		half_target.get(),
		blur_target.get());

	_targets.push_back(std::move(half_target));
	_targets.push_back(std::move(blur_target));
}

st_bloom_render_pass::~st_bloom_render_pass()
{
}

void st_bloom_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_bloom_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, _targets[0]->get_width(), _targets[0]->get_height());
	context->set_pipeline(_pipeline.get());

	_material->bind(context, params, identity, identity, identity);

	st_vec4f clears[] =
	{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
	};

	context->begin_render_pass(_pass.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	draw_call._geometry = _fullscreen_quad.get();
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get());

	_blur_pass->render(context, params);
}
