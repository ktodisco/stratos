/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_gbuffer_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_gbuffer_material::st_gbuffer_material(
	const char* albedo_texture,
	const char* mre_texture)
{
	st_graphics_context* context = st_graphics_context::get();
	_gbuffer_buffer = context->create_buffer(1, sizeof(st_gbuffer_cb), e_st_buffer_usage::uniform);
	context->add_constant(_gbuffer_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_albedo_texture = st_texture_loader::load(albedo_texture);
	context->set_texture_meta(_albedo_texture.get(), "SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_mre_texture = st_texture_loader::load(mre_texture);
	context->set_texture_meta(_mre_texture.get(), "SPIRV_Cross_Combinedmre_texturemre_sampler");

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _gbuffer_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);

	st_texture* textures[] = {
		_albedo_texture.get(),
		_mre_texture.get()
	};
	context->set_textures(_resource_table.get(), std::size(textures), textures);
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
	st_graphics_context* context,
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
	context->update_buffer(_gbuffer_buffer.get(), &gbuffer_cb, 1);

	context->bind_resource_table(_resource_table.get());
}
