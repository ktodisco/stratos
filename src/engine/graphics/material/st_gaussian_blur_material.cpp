/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_gaussian_blur_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

struct st_gaussian_blur_cb
{
	st_vec4f _source_dim;
};

st_gaussian_blur_vertical_material::st_gaussian_blur_vertical_material(
	st_render_texture* texture,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::gaussian),
	_texture(texture)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_gaussian_blur_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_gaussian_blur_vertical);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._vertex_format = vertex_format;
		desc._pass = pass;
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target->get_format();

		_pipeline = context->create_graphics_pipeline(desc);
	}

	_resource_table = context->create_resource_table();
	const st_texture_view* t = _texture->get_resource_view();
	const st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, &t, samplers);
	const st_buffer_view* cbs[] = { _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_gaussian_blur_vertical_material::~st_gaussian_blur_vertical_material()
{
	_cb = nullptr;
	_cbv = nullptr;
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_gaussian_blur_vertical_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_gaussian_blur_cb data;
	data._source_dim =
	{
		float(_texture->get_width()),
		float(_texture->get_height()),
		1.0f / _texture->get_width(),
		1.0f / _texture->get_height(),
	};
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->transition(_texture->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());
}

st_gaussian_blur_horizontal_material::st_gaussian_blur_horizontal_material(
	st_render_texture* texture,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::gaussian),
	_texture(texture)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_gaussian_blur_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_gaussian_blur_horizontal);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._vertex_format = vertex_format;
		desc._pass = pass;
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target->get_format();

		_pipeline = context->create_graphics_pipeline(desc);
	}

	_resource_table = context->create_resource_table();
	const st_texture_view* t = _texture->get_resource_view();
	const st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, &t, samplers);
	const st_buffer_view* cbs[] = { _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_gaussian_blur_horizontal_material::~st_gaussian_blur_horizontal_material()
{
	_cb = nullptr;
	_cbv = nullptr;
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_gaussian_blur_horizontal_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_gaussian_blur_cb data;
	data._source_dim =
	{
		float(_texture->get_width()),
		float(_texture->get_height()),
		1.0f / _texture->get_width(),
		1.0f / _texture->get_height(),
	};
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->transition(_texture->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());
}
