/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_parallax_occlusion_material.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>
#include <framework/st_output.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <cassert>
#include <iostream>

st_parallax_occlusion_material::st_parallax_occlusion_material(
	const char* albedo_texture,
	const char* normal_texture) :
	st_material(e_st_render_pass_type::gbuffer)
{
	st_device* device = st_output::get_device();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_parallax_occlusion_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_parallax_occlusion_buffer = device->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _parallax_occlusion_buffer.get();
		_pobv = device->create_buffer_view(desc);
	}

	_albedo_texture = st_texture_loader::load(albedo_texture);
	_normal_texture = st_texture_loader::load(normal_texture);

	{
		st_texture_desc albedo_desc;
		device->get_desc(_albedo_texture.get(), &albedo_desc);
		st_texture_view_desc desc;
		desc._texture = _albedo_texture.get();
		desc._format = albedo_desc._format;
		desc._first_mip = 0;
		desc._mips = albedo_desc._levels;
		_albedo_view = device->create_texture_view(desc);
	}

	{
		st_texture_desc normal_desc;
		device->get_desc(_normal_texture.get(), &normal_desc);
		st_texture_view_desc desc;
		desc._texture = _normal_texture.get();
		desc._format = normal_desc._format;
		desc._first_mip = 0;
		desc._mips = normal_desc._levels;
		_normal_view = device->create_texture_view(desc);
	}

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_normal, st_format_r32g32b32_float, 1));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_tangent, st_format_r32g32b32_float, 2));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32a32_float, 3));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_uv, st_format_r32g32_float, 4));
	_vertex_format = device->create_vertex_format(attributes.data(), attributes.size());

	st_output* output = st_output::get();

	st_graphics_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_parallax_occlusion);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._blend_desc._target_blend[1]._blend = false;
	desc._depth_stencil_desc._depth_enable = true;
	desc._depth_stencil_desc._depth_compare = e_st_compare_func::st_compare_func_less;
	output->get_target_formats(e_st_render_pass_type::gbuffer, desc);

	_pipeline = device->create_graphics_pipeline(desc);

	_resource_table = device->create_resource_table();
	const st_buffer_view* cbs[] = { _pobv.get() };
	device->set_constant_buffers(_resource_table.get(), 1, cbs);
	const st_texture_view* textures[] = {
		_albedo_view.get(),
		_normal_view.get()
	};
	const st_sampler* samplers[] = {
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get(),
	};
	device->set_textures(_resource_table.get(), std::size(textures), textures, samplers);
}

st_parallax_occlusion_material::~st_parallax_occlusion_material()
{
	_pipeline = nullptr;
	_vertex_format = nullptr;
	_resource_table = nullptr;
	_pobv = nullptr;
	_parallax_occlusion_buffer = nullptr;

	_albedo_view = nullptr;
	_normal_view = nullptr;
	_albedo_texture = nullptr;
	_normal_texture = nullptr;
}

void st_parallax_occlusion_material::bind(
	st_command_list* command_list,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	command_list->set_pipeline(_pipeline.get());

	st_mat4f mvp = transform * view * proj;
	st_mat4f transform_t = transform;

	st_parallax_occlusion_cb pom_cb{};
	pom_cb._model = transform_t;
	pom_cb._mvp = mvp;
	pom_cb._eye = st_vec4f(params->_eye, 0.0f);
	command_list->update_buffer(_parallax_occlusion_buffer.get(), &pom_cb, 0, 1);

	command_list->bind_resources(_resource_table.get());
}
