/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_gbuffer_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>

#include <cassert>
#include <iostream>

st_gbuffer_material::st_gbuffer_material(
	const char* albedo_texture,
	const char* mre_texture)
{
	_gbuffer_buffer = std::make_unique<st_constant_buffer>(sizeof(st_gbuffer_cb));
	_gbuffer_buffer->add_constant("type_cb0", st_shader_constant_type_block);

	_albedo_texture = std::make_unique<st_texture>();
	if (!_albedo_texture->load_from_file(albedo_texture))
	{
		std::cerr << "Failed to load " << albedo_texture << std::endl;
		assert(false);
	}
	_albedo_texture->set_meta("SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_mre_texture = std::make_unique<st_texture>();
	if (!_mre_texture->load_from_file(mre_texture))
	{
		std::cerr << "Failed to load " << mre_texture << std::endl;
		assert(false);
	}
	_mre_texture->set_meta("SPIRV_Cross_Combinedmre_texturemre_sampler");

	_resource_table = std::make_unique<st_resource_table>();
	_resource_table->add_constant_buffer(_gbuffer_buffer.get());
	_resource_table->add_texture_resource(_albedo_texture.get());
	_resource_table->add_texture_resource(_mre_texture.get());
}

st_gbuffer_material::~st_gbuffer_material()
{
}

void st_gbuffer_material::get_pipeline_state(
	st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_gbuffer);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_blend_desc._target_blend[1]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}

void st_gbuffer_material::bind(
	st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_gbuffer_cb gbuffer_cb{};
	gbuffer_cb._model = transform;
	gbuffer_cb._mvp = mvp;
	gbuffer_cb._emissive = _emissive;
	_gbuffer_buffer->update(context, &gbuffer_cb);

	_resource_table->bind(context);
}
