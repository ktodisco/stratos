/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_unlit_texture_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_unlit_texture_material::st_unlit_texture_material(const char* texture_file) :
	st_material(e_st_render_pass_type::gbuffer),
	_texture_file(texture_file)
{
	st_graphics_context* context = st_graphics_context::get();

	st_buffer_desc desc;
	desc._count = 1;
	desc._element_size = sizeof(st_view_cb);
	desc._usage = e_st_buffer_usage::uniform;
	_view_buffer = context->create_buffer(desc);
	context->add_constant(_view_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_texture = st_texture_loader::load(_texture_file.c_str());

	//state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_unlit_texture);
	//state_desc->_blend_desc._target_blend[0]._blend = false;
	//state_desc->_depth_stencil_desc._depth_enable = true;
	//state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _view_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
	st_texture* textures[] = { _texture.get() };
	st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, textures, samplers);
}

st_unlit_texture_material::~st_unlit_texture_material()
{
}

void st_unlit_texture_material::bind(
	st_graphics_context* context,
	enum e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	context->update_buffer(_view_buffer.get(), &cb_data, 0, 1);

	context->bind_resource_table(_resource_table.get());
}
