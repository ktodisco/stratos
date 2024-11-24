/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_gbuffer_material.h>

#include <framework/st_global_resources.h>
#include <framework/st_output.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_gbuffer_material::st_gbuffer_material(
	const char* albedo_texture,
	const char* mre_texture) :
	st_material(e_st_render_pass_type::shadow | e_st_render_pass_type::gbuffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_gbuffer_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_gbuffer_buffer = context->create_buffer(desc);
		context->add_constant(_gbuffer_buffer.get(), "type_cb0", st_shader_constant_type_block);
	}

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_shadow_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_shadow_buffer = context->create_buffer(desc);
		context->add_constant(_shadow_buffer.get(), "type_cb0", st_shader_constant_type_block);
	}

	_albedo_texture = st_texture_loader::load(albedo_texture);
	_mre_texture = st_texture_loader::load(mre_texture);

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_normal, st_format_r32g32b32_float, 1));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_tangent, st_format_r32g32b32_float, 2));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32a32_float, 3));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_uv, st_format_r32g32_float, 4));
	_vertex_format = context->create_vertex_format(attributes.data(), attributes.size());

	st_output* output = st_output::get();

	{
		st_pipeline_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_gbuffer);
		desc._vertex_format = _vertex_format.get();
		desc._blend_desc._target_blend[0]._blend = false;
		desc._blend_desc._target_blend[1]._blend = false;
		desc._depth_stencil_desc._depth_enable = true;
		desc._depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
		output->get_target_formats(e_st_render_pass_type::gbuffer, desc);

		_gbuffer_pipeline = context->create_pipeline(desc);
	}
	{
		st_pipeline_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_shadow);
		desc._vertex_format = _vertex_format.get();
		desc._depth_stencil_desc._depth_enable = true;
		desc._depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
		output->get_target_formats(e_st_render_pass_type::shadow, desc);

		_shadow_pipeline = context->create_pipeline(desc);
	}

	{
		_gbuffer_resources = context->create_resource_table();
		st_buffer* cbs[] = { _gbuffer_buffer.get() };
		context->set_constant_buffers(_gbuffer_resources.get(), 1, cbs);

		st_texture* textures[] = {
			_albedo_texture.get(),
			_mre_texture.get()
		};
		st_sampler* samplers[] = {
			_global_resources->_trilinear_wrap_sampler.get(),
			_global_resources->_trilinear_wrap_sampler.get(),
		};
		context->set_textures(_gbuffer_resources.get(), std::size(textures), textures, samplers);
	}
	{
		_shadow_resources = context->create_resource_table();
		st_buffer* cbs[] = { _shadow_buffer.get() };
		context->set_constant_buffers(_shadow_resources.get(), 1, cbs);
	}
}

st_gbuffer_material::~st_gbuffer_material()
{
	_gbuffer_pipeline = nullptr;
	_shadow_pipeline = nullptr;
	_vertex_format = nullptr;
	_gbuffer_resources = nullptr;
	_shadow_resources = nullptr;
}

void st_gbuffer_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	st_mat4f mvp = transform * view * proj;

	if (pass_type == e_st_render_pass_type::shadow)
	{
		context->set_pipeline(_shadow_pipeline.get());

		st_shadow_cb shadow_cb{};
		shadow_cb._mvp = mvp;
		context->update_buffer(_shadow_buffer.get(), &shadow_cb, 0, 1);

		context->bind_resource_table(_shadow_resources.get());
	}
	else if (pass_type == e_st_render_pass_type::gbuffer)
	{
		context->set_pipeline(_gbuffer_pipeline.get());

		st_gbuffer_cb gbuffer_cb{};
		gbuffer_cb._model = transform;
		gbuffer_cb._mvp = mvp;
		gbuffer_cb._emissive = _emissive;
		context->update_buffer(_gbuffer_buffer.get(), &gbuffer_cb, 0, 1);

		context->bind_resource_table(_gbuffer_resources.get());
	}
}
