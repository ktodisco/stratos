/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_gbuffer_render_pass.h>

#include <framework/st_frame_params.h>

#include <graphics/geometry/st_vertex_format.h>
#include <graphics/material/st_gbuffer_material.h>
#include <graphics/material/st_parallax_occlusion_material.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_framebuffer.h>
#include <graphics/st_pipeline_state.h>
#include <graphics/st_render_context.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>

st_gbuffer_render_pass::st_gbuffer_render_pass(
	st_render_texture* albedo_buffer,
	st_render_texture* normal_buffer,
	st_render_texture* third_buffer,
	st_render_texture* depth_buffer)
{
	_vertex_format = std::make_unique<st_vertex_format>();
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_normal, 1));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_tangent, 2));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_color, 3));
	_vertex_format->add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 4));
	_vertex_format->finalize();

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

	_gbuffer_state = std::make_unique<st_pipeline_state>(gbuffer_state_desc);

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

	_parallax_occlusion_state = std::make_unique<st_pipeline_state>(parallax_occlusion_state_desc);

	st_render_texture* targets[] = { albedo_buffer, normal_buffer, third_buffer };
	_framebuffer = std::make_unique<st_framebuffer>(
		3,
		targets,
		depth_buffer);
}

st_gbuffer_render_pass::~st_gbuffer_render_pass()
{
}

void st_gbuffer_render_pass::render(st_render_context* context, const st_frame_params* params)
{
	st_render_marker marker("st_gbuffer_render_pass::render");

	// Compute projection matrices.
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	context->set_viewport(0, 0, params->_width, params->_height);
	context->set_scissor(0, 0, params->_width, params->_height);
	context->set_pipeline_state(_gbuffer_state.get());

	_framebuffer->bind(context);

	// Clear viewport.
	context->set_clear_color(0.0f, 0.0f, 0.0f, 1.0f);
	context->clear(st_clear_flag_color | st_clear_flag_depth);

	// Draw all static geometry.
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(d._name.c_str());

		if (!d._material)
		{
			context->set_pipeline_state(_gbuffer_state.get());
			_default_gbuffer->bind(context, params, perspective, params->_view, d._transform);
		}
		else
		{
			switch (d._material->get_material_type())
			{
			case st_material_type_parallax_occlusion:
				context->set_pipeline_state(_parallax_occlusion_state.get());
				break;
			case st_material_type_gbuffer:
			default:
				context->set_pipeline_state(_gbuffer_state.get());
				break;
			}

			d._material->bind(context, params, perspective, params->_view, d._transform);
		}

		context->draw(d);
	}

	_framebuffer->unbind(context);
}
