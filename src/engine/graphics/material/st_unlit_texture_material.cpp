/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_unlit_texture_material.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_unlit_texture_material::st_unlit_texture_material(const char* texture_file) :
	_texture_file(texture_file)
{
	st_render_context* context = st_render_context::get();
	_view_buffer = context->create_buffer(1, sizeof(st_view_cb), e_st_buffer_usage::uniform);
	context->add_constant(_view_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_texture = st_texture_loader::load(_texture_file.c_str());
	context->set_texture_meta(_texture.get(), "SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _view_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
	st_texture* textures[] = { _texture.get() };
	context->set_textures(_resource_table.get(), 1, textures);
}

st_unlit_texture_material::~st_unlit_texture_material()
{
}

void st_unlit_texture_material::get_pipeline_state(
	struct st_pipeline_state_desc* state_desc)
{
	state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_unlit_texture);

	state_desc->_blend_desc._target_blend[0]._blend = false;
	state_desc->_depth_stencil_desc._depth_enable = true;
	state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
}

void st_unlit_texture_material::bind(
	st_render_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	context->update_buffer(_view_buffer.get(), &cb_data, 1);

	context->bind_resource_table(_resource_table.get());
}
