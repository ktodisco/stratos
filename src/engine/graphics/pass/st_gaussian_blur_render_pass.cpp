/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_gaussian_blur_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/material/st_gaussian_blur_material.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_pass.h>
#include <graphics/st_render_texture.h>

st_gaussian_blur_render_pass::st_gaussian_blur_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* target_buffer)
{
	// Set up the intermediate render target between the two blur passes.
	_intermediate_target = std::make_unique<st_render_texture>(
		source_buffer->get_width(),
		source_buffer->get_height(),
		source_buffer->get_format(),
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f({ 0.0f, 0.0f, 0.0f, 0.0f }));
	_intermediate_target->set_name("Gaussian Blur Intermediate");

	st_render_texture* vertical_blur_targets[] = { _intermediate_target.get() };
	_vertical_blur_pass = std::make_unique<st_render_pass>(
		1,
		vertical_blur_targets,
		nullptr);

	// TODO: Need to pass the inverse texture sizes to the shaders in these materials.
	_vertical_blur_material = std::make_unique<st_gaussian_blur_vertical_material>(source_buffer);
	_horizontal_blur_material = std::make_unique<st_gaussian_blur_horizontal_material>(_intermediate_target.get());

	// Set up the vertical blur pipeline state.
	st_pipeline_state_desc vertical_blur_state_desc;
	_vertical_blur_material->get_pipeline_state(&vertical_blur_state_desc);

	vertical_blur_state_desc._vertex_format = _vertex_format.get();
	vertical_blur_state_desc._render_target_count = 1;
	vertical_blur_state_desc._render_target_formats[0] = target_buffer->get_format();

	_vertical_blur_state = std::make_unique<st_pipeline_state>(
		vertical_blur_state_desc,
		_vertical_blur_pass.get());

	st_render_texture* horizontal_blur_targets[] = { target_buffer };
	_horizontal_blur_pass = std::make_unique<st_render_pass>(
		1,
		horizontal_blur_targets,
		nullptr);

	// Set up the horizontal blur pipeline state.
	st_pipeline_state_desc horizontal_blur_state_desc;
	_horizontal_blur_material->get_pipeline_state(&horizontal_blur_state_desc);

	horizontal_blur_state_desc._vertex_format = _vertex_format.get();
	horizontal_blur_state_desc._render_target_count = 1;
	horizontal_blur_state_desc._render_target_formats[0] = target_buffer->get_format();

	_horizontal_blur_state = std::make_unique<st_pipeline_state>(
		horizontal_blur_state_desc,
		_horizontal_blur_pass.get());
}

st_gaussian_blur_render_pass::~st_gaussian_blur_render_pass()
{
}

void st_gaussian_blur_render_pass::render(
	st_render_context* context,
	const st_frame_params* params)
{
	st_render_marker marker("st_gaussian_blur_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	context->set_viewport(0, 0, _intermediate_target->get_width(), _intermediate_target->get_height());
	context->set_scissor(0, 0, _intermediate_target->get_width(), _intermediate_target->get_height());

	{
		st_render_marker marker("Vertical Blur");

		context->set_pipeline_state(_vertical_blur_state.get());

		_vertical_blur_material->bind(context, params, identity, identity, identity);

		st_vec4f clears[] =
		{
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		_vertical_blur_pass->begin(context, clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);

		context->draw(draw_call);

		_vertical_blur_pass->end(context);
	}

	{
		st_render_marker marker("Horizontal Blur");

		context->set_pipeline_state(_horizontal_blur_state.get());

		_horizontal_blur_material->bind(context, params, identity, identity, identity);

		st_vec4f clears[] =
		{
			{ 0.0f, 0.0f, 0.0f, 1.0f },
		};
		_horizontal_blur_pass->begin(context, clears, std::size(clears));

		st_static_drawcall draw_call;
		draw_call._name = "fullscreen_quad";
		draw_call._transform = identity;
		_fullscreen_quad->draw(draw_call);

		context->draw(draw_call);

		_horizontal_blur_pass->end(context);
	}
}
