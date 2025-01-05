/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_tonemap_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

st_tonemap_material::st_tonemap_material(
	st_render_texture* texture,
	st_render_texture* bloom,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::tonemap),
	_texture(texture),
	_bloom(bloom)
{
	st_graphics_context* context = st_graphics_context::get();

	st_graphics_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_tonemap);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._depth_stencil_desc._depth_enable = false;
	desc._vertex_format = vertex_format;
	desc._pass = pass;
	desc._render_target_count = 1;
	desc._render_target_formats[0] = target->get_format();

	_pipeline = context->create_graphics_pipeline(desc);

	_resource_table = context->create_resource_table();
	st_texture* textures[] =
	{
		_texture->get_texture(),
		_bloom->get_texture()
	};
	st_sampler* samplers[] =
	{
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get()
	};
	context->set_textures(_resource_table.get(), 2, textures, samplers);
}

st_tonemap_material::~st_tonemap_material()
{
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_tonemap_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	context->transition(_texture->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_bloom->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resource_table(_resource_table.get());
}
