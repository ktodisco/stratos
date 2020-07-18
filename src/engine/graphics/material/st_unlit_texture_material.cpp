/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_unlit_texture_material.h>

#include <graphics/st_constant_buffer.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_context.h>
#include <graphics/st_resource_table.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_unlit_texture_material::st_unlit_texture_material(const char* texture_file) :
	_texture_file(texture_file)
{
	_view_buffer = std::make_unique<st_constant_buffer>(sizeof(st_view_cb));
	_view_buffer->add_constant("type_cb0", st_shader_constant_type_block);

	_texture = st_texture_loader::load(_texture_file.c_str());
	_texture->set_meta("SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_resource_table = std::make_unique<st_resource_table>();
	st_constant_buffer* cbs[] = { _view_buffer.get() };
	_resource_table->set_constant_buffers(1, cbs);
	st_texture* textures[] = { _texture.get() };
	_resource_table->set_textures(1, textures);
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
	_view_buffer->update(context, &cb_data);

	_resource_table->bind(context);
}
