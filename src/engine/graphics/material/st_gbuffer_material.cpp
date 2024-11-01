/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_gbuffer_material.h>

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
	st_material(e_st_render_pass_type::gbuffer)
{
	st_graphics_context* context = st_graphics_context::get();
	_gbuffer_buffer = context->create_buffer(1, sizeof(st_gbuffer_cb), e_st_buffer_usage::uniform);
	context->add_constant(_gbuffer_buffer.get(), "type_cb0", st_shader_constant_type_block);

	_albedo_texture = st_texture_loader::load(albedo_texture);
	context->set_texture_meta(_albedo_texture.get(), "SPIRV_Cross_Combineddiffuse_texturediffuse_sampler");

	_mre_texture = st_texture_loader::load(mre_texture);
	context->set_texture_meta(_mre_texture.get(), "SPIRV_Cross_Combinedmre_texturemre_sampler");

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_normal, st_format_r32g32b32_float, 1));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_tangent, st_format_r32g32b32_float, 2));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32a32_float, 3));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_uv, st_format_r32g32_float, 4));
	_vertex_format = context->create_vertex_format(attributes.data(), attributes.size());

	st_output* output = st_output::get();

	st_pipeline_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_gbuffer);
	desc._vertex_format = _vertex_format.get();
	desc._blend_desc._target_blend[0]._blend = false;
	desc._blend_desc._target_blend[1]._blend = false;
	desc._depth_stencil_desc._depth_enable = true;
	desc._depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
	output->get_target_formats(e_st_render_pass_type::gbuffer, desc);

	_pipeline = context->create_pipeline(desc);

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
	_pipeline = nullptr;
	_vertex_format = nullptr;
	_resource_table = nullptr;
}

void st_gbuffer_material::bind(
	st_graphics_context* context,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_mat4f mvp = transform * view * proj;
	mvp.transpose();

	st_gbuffer_cb gbuffer_cb{};
	gbuffer_cb._model = transform;
	gbuffer_cb._mvp = mvp;
	gbuffer_cb._emissive = _emissive;
	context->update_buffer(_gbuffer_buffer.get(), &gbuffer_cb, 0, 1);

	context->bind_resource_table(_resource_table.get());
}
