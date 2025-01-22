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

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_view_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_view_buffer = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _view_buffer.get();
		_vbv = context->create_buffer_view(desc);
	}

	_texture = st_texture_loader::load(_texture_file.c_str());

	{
		st_texture_desc texture_desc;
		context->get_desc(_texture.get(), &texture_desc);
		st_texture_view_desc desc;
		desc._texture = _texture.get();
		desc._format = texture_desc._format;
		desc._first_mip = 0;
		desc._mips = texture_desc._levels;
		_view = context->create_texture_view(desc);
	}

	//state_desc->_shader = st_shader_manager::get()->get_shader(st_shader_unlit_texture);
	//state_desc->_blend_desc._target_blend[0]._blend = false;
	//state_desc->_depth_stencil_desc._depth_enable = true;
	//state_desc->_depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;

	_resource_table = context->create_resource_table();
	const st_buffer_view* cbs[] = { _vbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
	const st_texture_view* textures[] = { _view.get() };
	const st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, textures, samplers);
}

st_unlit_texture_material::~st_unlit_texture_material()
{
	_resource_table = nullptr;
	_vbv = nullptr;
	_view_buffer = nullptr;
	_texture = nullptr;
	_view = nullptr;
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

	st_view_cb cb_data{};
	cb_data._mvp = mvp;
	context->update_buffer(_view_buffer.get(), &cb_data, 0, 1);

	context->bind_resources(_resource_table.get());
}
