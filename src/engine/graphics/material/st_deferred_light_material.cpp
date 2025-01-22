/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_deferred_light_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

st_deferred_light_material::st_deferred_light_material(
	st_render_texture* albedo_texture,
	st_render_texture* normal_texture,
	st_render_texture* third_texture,
	st_render_texture* depth_texture,
	st_render_texture* directional_shadow_map,
	st_render_texture* output_texture,
	const st_buffer_view* constants,
	const st_buffer_view* light_buffer,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::deferred),
	_albedo(albedo_texture),
	_normal(normal_texture),
	_third(third_texture),
	_depth(depth_texture),
	_directional_shadow_map(directional_shadow_map)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_deferred_light);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._vertex_format = vertex_format;
		desc._pass = pass;
		desc._render_target_count = 1;
		desc._render_target_formats[0] = output_texture->get_format();

		_pipeline = context->create_graphics_pipeline(desc);
	}

	_resource_table = context->create_resource_table();
	context->set_constant_buffers(_resource_table.get(), 1, &constants);
	context->set_buffers(_resource_table.get(), 1, &light_buffer);

	{
		st_sampler_desc desc;
		desc._min_filter = st_filter_nearest;
		desc._mag_filter = st_filter_nearest;
		desc._mip_filter = st_filter_nearest;
		desc._address_u = st_address_mode_wrap;
		desc._address_v = st_address_mode_wrap;

		_shadow_sampler = context->create_sampler(desc);
	}

	const st_texture_view* textures[] = {
		_albedo->get_resource_view(),
		_normal->get_resource_view(),
		_third->get_resource_view(),
		_depth->get_resource_view(),
		_directional_shadow_map->get_resource_view(),
	};
	const st_sampler* samplers[] = {
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get(),
		_shadow_sampler.get(),
	};
	context->set_textures(_resource_table.get(), std::size(textures), textures, samplers);
}

st_deferred_light_material::~st_deferred_light_material()
{
	_pipeline = nullptr;
	_shadow_sampler = nullptr;
	_resource_table = nullptr;
}

void st_deferred_light_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	context->transition(_albedo->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_normal->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_third->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_depth->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_directional_shadow_map->get_texture(), st_texture_state_pixel_shader_read);

	context->bind_resources(_resource_table.get());
}
