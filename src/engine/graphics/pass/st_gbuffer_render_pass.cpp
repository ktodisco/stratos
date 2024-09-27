/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_gbuffer_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/material/st_gbuffer_material.h>
#include <graphics/material/st_parallax_occlusion_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_gbuffer_render_pass::st_gbuffer_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* third_buffer,
	st_render_texture* depth_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	st_target_desc targets[] =
	{
		{ albedo_buffer, e_st_load_op::clear, e_st_store_op::store },
		{ normal_buffer, e_st_load_op::clear, e_st_store_op::store },
		{ third_buffer, e_st_load_op::clear, e_st_store_op::store },
	};
	st_target_desc ds_target = { depth_buffer, e_st_load_op::clear, e_st_store_op::store };
	_pass = context->create_render_pass(
		3,
		targets,
		&ds_target);

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_normal, st_format_r32g32b32_float, 1));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_tangent, st_format_r32g32b32_float, 2));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32a32_float, 3));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_uv, st_format_r32g32_float, 4));
	_vertex_format = context->create_vertex_format(attributes.data(), attributes.size());

	// Set up the gbuffer material and state.
	_default_gbuffer = std::make_unique<st_gbuffer_material>(
		"data/textures/default_albedo.png",
		"data/textures/default_mre.png");

	st_pipeline_state_desc gbuffer_state_desc;
	_default_gbuffer->get_pipeline_state(&gbuffer_state_desc);

	gbuffer_state_desc._vertex_format = _vertex_format.get();
	gbuffer_state_desc._render_target_count = 3;
	gbuffer_state_desc._render_target_formats[0] = albedo_buffer->get_format();
	gbuffer_state_desc._render_target_formats[1] = normal_buffer->get_format();
	gbuffer_state_desc._render_target_formats[2] = third_buffer->get_format();
	gbuffer_state_desc._depth_stencil_format = depth_buffer->get_format();

	_gbuffer_state = context->create_pipeline(gbuffer_state_desc, _pass.get());

	// TODO: See comment in header regarding multiple materials in a single render pass.
	_default_parallax_occlusion = std::make_unique<st_parallax_occlusion_material>(
		"data/textures/default_albedo.png",
		"data/textures/pom_normal.png");

	st_pipeline_state_desc parallax_occlusion_state_desc;
	_default_parallax_occlusion->get_pipeline_state(&parallax_occlusion_state_desc);

	parallax_occlusion_state_desc._vertex_format = _vertex_format.get();
	parallax_occlusion_state_desc._render_target_count = 3;
	parallax_occlusion_state_desc._render_target_formats[0] = albedo_buffer->get_format();
	parallax_occlusion_state_desc._render_target_formats[1] = normal_buffer->get_format();
	parallax_occlusion_state_desc._render_target_formats[2] = third_buffer->get_format();
	parallax_occlusion_state_desc._depth_stencil_format = depth_buffer->get_format();

	_parallax_occlusion_state = context->create_pipeline(parallax_occlusion_state_desc, _pass.get());
}

st_gbuffer_render_pass::~st_gbuffer_render_pass()
{
}

void st_gbuffer_render_pass::render(st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, "st_gbuffer_render_pass::render");

	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline(_gbuffer_state.get());

	st_vec4f clears[] =
	{
		{ 0.0f, 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f, 0.0f }
	};

	context->begin_render_pass(_pass.get(), clears, std::size(clears));

	// Clear viewport.
	context->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	context->clear(st_clear_flag_color | st_clear_flag_depth);

	// Draw all static geometry.
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(context, d._name.c_str());

		if (!d._material)
		{
			context->set_pipeline(_gbuffer_state.get());
			_default_gbuffer->bind(context, params, params->_projection, params->_view, d._transform);
		}
		else
		{
			switch (d._material->get_material_type())
			{
			case st_material_type_parallax_occlusion:
				context->set_pipeline(_parallax_occlusion_state.get());
				break;
			case st_material_type_gbuffer:
			default:
				context->set_pipeline(_gbuffer_state.get());
				break;
			}

			d._material->bind(context, params, params->_projection, params->_view, d._transform);
		}

		context->draw(d);
	}

	context->end_render_pass(_pass.get());
}
