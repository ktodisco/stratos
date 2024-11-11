/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_bloom_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

st_bloom_material::st_bloom_material(
	st_render_texture* source,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::bloom),
	_texture(source->get_texture())
{
	st_graphics_context* context = st_graphics_context::get();

	st_pipeline_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_bloom);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._depth_stencil_desc._depth_enable = false;
	desc._vertex_format = vertex_format;
	desc._pass = pass;
	desc._render_target_count = 1;
	desc._render_target_formats[0] = target->get_format();

	_pipeline = context->create_pipeline(desc);

	_resource_table = context->create_resource_table();
	st_sampler* samplers[] { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, &_texture, samplers);
}

st_bloom_material::~st_bloom_material()
{
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_bloom_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	context->set_texture_meta(_texture, "SPIRV_Cross_Combinedtextex_sampler");
	context->transition(_texture, st_texture_state_pixel_shader_read);
	context->bind_resource_table(_resource_table.get());
}
