/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_bloom_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/material/st_bloom_material.h>
#include <graphics/pass/st_gaussian_blur_render_pass.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_bloom_render_pass::st_bloom_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		_threshold_target = std::make_unique<st_render_texture>(
			context,
			source_buffer->get_width(),
			source_buffer->get_height(),
			source_buffer->get_format(),
			e_st_texture_usage::color_target | e_st_texture_usage::sampled,
			st_texture_state_pixel_shader_read,
			st_vec4f::zero_vector(),
			"Bloom Threshold");

		st_target_desc targets[] =
		{
			{ _threshold_target.get(), e_st_load_op::clear, e_st_store_op::store }
		};
		_threshold_pass = context->create_render_pass(1, targets, nullptr);

		_threshold_material = std::make_unique<st_bloom_threshold_material>(
			source_buffer,
			_threshold_target.get(),
			_vertex_format.get(),
			_threshold_pass.get(),
			100.0f);
	}

	for (uint32_t d = 0; d < k_num_downsamples; ++d)
	{
		_downsample_targets[d] = std::make_unique<st_render_texture>(
			context,
			source_buffer->get_width() >> (d + 1),
			source_buffer->get_height() >> (d + 1),
			source_buffer->get_format(),
			e_st_texture_usage::color_target | e_st_texture_usage::sampled,
			st_texture_state_pixel_shader_read,
			st_vec4f::zero_vector(),
			"Bloom Downsample");

		uint32_t u = (k_num_downsamples - 1 - d);
		_upsample_targets[u] = std::make_unique<st_render_texture>(
			context,
			source_buffer->get_width() >> (d + 1),
			source_buffer->get_height() >> (d + 1),
			source_buffer->get_format(),
			e_st_texture_usage::color_target | e_st_texture_usage::sampled,
			st_texture_state_pixel_shader_read,
			st_vec4f::zero_vector(),
			"Bloom Upsample");
	}

	// Create the downsample passes.
	for (uint32_t d = 0; d < k_num_downsamples; ++d)
	{
		st_target_desc targets[] =
		{
			{ _downsample_targets[d].get(), e_st_load_op::clear, e_st_store_op::store }
		};
		_downsample_passes[d] = context->create_render_pass(1, targets, nullptr);

		_downsample_materials[d] = std::make_unique<st_bloom_downsample_material>(
			(d == 0) ? _threshold_target.get() : _downsample_targets[d - 1].get(),
			_downsample_targets[d].get(),
			_vertex_format.get(),
			_downsample_passes[d].get());
	}

	// Create the blur passes.
	for (uint32_t b = 0; b < k_num_downsamples; ++b)
	{
		st_render_texture* target = nullptr;
		if (b == 0)
			target = _upsample_targets[0].get();
		else if (b == (k_num_downsamples - 1))
			target = target_buffer;
		else
			target = _downsample_targets[k_num_downsamples - b - 1].get();
		_blur_passes[b] = std::make_unique<st_gaussian_blur_render_pass>(
			(b == 0) ? _downsample_targets[k_num_downsamples - 1].get() : _upsample_targets[b].get(),
			target);
	}

	// Create the upsample passes.
	for (uint32_t u = 0; u < (k_num_downsamples - 1); ++u)
	{
		st_target_desc targets[] =
		{
			{ _upsample_targets[u + 1].get(), e_st_load_op::clear, e_st_store_op::store }
		};
		_upsample_passes[u] = context->create_render_pass(1, targets, nullptr);

		_upsample_materials[u] = std::make_unique<st_bloom_upsample_material>(
			(u == 0) ? _upsample_targets[u].get() : _downsample_targets[k_num_downsamples - 1 - u].get(),
			_downsample_targets[k_num_downsamples - 2 - u].get(),
			_upsample_targets[u + 1].get(),
			_vertex_format.get(),
			_upsample_passes[u].get());
	}
}

st_bloom_render_pass::~st_bloom_render_pass()
{
	for (auto& p : _downsample_passes)
		p = nullptr;
	for (auto& u : _upsample_passes)
		u = nullptr;
	for (auto& b : _blur_passes)
		b = nullptr;
	for (auto& t : _downsample_targets)
		t = nullptr;
	for (auto& t : _upsample_targets)
		t = nullptr;

	_threshold_target = nullptr;
	_threshold_material = nullptr;
	_threshold_pass = nullptr;
}

void st_bloom_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_bloom_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	// Threshold.
	{
		st_render_marker threshold_marker(context, "threshold pass");
		context->set_scissor(0, 0, _threshold_target->get_width(), _threshold_target->get_height());

		_threshold_material->bind(context, e_st_render_pass_type::bloom, params, identity, identity, identity);

		st_clear_value clears[] =
		{
			st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		context->begin_render_pass(_threshold_pass.get(), clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);
		draw_call._draw_mode = st_primitive_topology_triangles;

		context->draw(draw_call);

		context->end_render_pass(_threshold_pass.get());
	}

	// Downsamples.
	for (uint32_t d = 0; d < k_num_downsamples; ++d)
	{
		st_render_marker downsample_marker(context, "downsample pass");
		context->set_scissor(0, 0, _downsample_targets[d]->get_width(), _downsample_targets[d]->get_height());

		_downsample_materials[d]->bind(context, e_st_render_pass_type::bloom, params, identity, identity, identity);

		st_clear_value clears[] =
		{
			st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		context->begin_render_pass(_downsample_passes[d].get(), clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);
		draw_call._draw_mode = st_primitive_topology_triangles;

		context->draw(draw_call);

		context->end_render_pass(_downsample_passes[d].get());
	}

	// Blur and upsample.
	for (uint32_t u = 0; u < (k_num_downsamples - 1); ++u)
	{
		_blur_passes[u]->render(context, params);

		st_render_marker downsample_marker(context, "upsample pass");
		context->set_scissor(0, 0, _upsample_targets[u + 1]->get_width(), _upsample_targets[u + 1]->get_height());

		_upsample_materials[u]->bind(context, e_st_render_pass_type::bloom, params, identity, identity, identity);

		st_clear_value clears[] =
		{
			st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
		};

		context->begin_render_pass(_upsample_passes[u].get(), clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);
		draw_call._draw_mode = st_primitive_topology_triangles;

		context->draw(draw_call);

		context->end_render_pass(_upsample_passes[u].get());
	}

	// Finally, blur the last target.
	_blur_passes[k_num_downsamples - 1]->render(context, params);
}
