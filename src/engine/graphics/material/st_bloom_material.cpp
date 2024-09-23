/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_bloom_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>

st_bloom_material::st_bloom_material(st_texture* texture) :
	_texture(texture)
{
	st_graphics_context* context = st_graphics_context::get();
	_resource_table = context->create_resource_table();
	context->set_textures(_resource_table.get(), 1, &_texture);
}

st_bloom_material::~st_bloom_material()
{
}

void st_bloom_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_bloom);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = false;
}

void st_bloom_material::bind(
	st_graphics_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_texture_meta(_texture, "SPIRV_Cross_Combinedtextex_sampler");
	context->transition(_texture, st_texture_state_pixel_shader_read);
	context->bind_resource_table(_resource_table.get());
}
