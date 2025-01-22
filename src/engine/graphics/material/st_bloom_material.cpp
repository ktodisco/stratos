/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/material/st_bloom_material.h>

#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

struct st_bloom_threshold_cb
{
	float _cutoff;
	
	float _pad0;
	float _pad1;
	float _pad2;
};

struct st_bloom_downsample_cb
{
	st_vec4f _target_dim;
};

struct st_bloom_upsample_cb
{
	st_vec4f _source_dim;
};

st_bloom_threshold_material::st_bloom_threshold_material(
	st_render_texture* source,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass,
	float cutoff) :
	st_material(e_st_render_pass_type::bloom),
	_texture(source),
	_cutoff(cutoff)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_bloom_threshold_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	st_graphics_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_bloom_threshold);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._depth_stencil_desc._depth_enable = false;
	desc._vertex_format = vertex_format;
	desc._pass = pass;
	desc._render_target_count = 1;
	desc._render_target_formats[0] = target->get_format();

	_pipeline = context->create_graphics_pipeline(desc);

	_resource_table = context->create_resource_table();
	const st_texture_view* textures[]{ _texture->get_resource_view() };
	const st_sampler* samplers[]{ _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, textures, samplers);
	const st_buffer_view* cbs[]{ _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_bloom_threshold_material::~st_bloom_threshold_material()
{
	_cb = nullptr;
	_cbv = nullptr;
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_bloom_threshold_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_bloom_threshold_cb data;
	data._cutoff = _cutoff;
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->transition(_texture->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());
}

st_bloom_downsample_material::st_bloom_downsample_material(
	st_render_texture* source,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::bloom),
	_texture(source)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_bloom_downsample_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	st_graphics_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_bloom_downsample);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._depth_stencil_desc._depth_enable = false;
	desc._vertex_format = vertex_format;
	desc._pass = pass;
	desc._render_target_count = 1;
	desc._render_target_formats[0] = target->get_format();

	_pipeline = context->create_graphics_pipeline(desc);

	_resource_table = context->create_resource_table();
	const st_texture_view* textures[] { _texture->get_resource_view() };
	const st_sampler* samplers[] { _global_resources->_trilinear_clamp_sampler.get() };
	context->set_textures(_resource_table.get(), 1, textures, samplers);
	const st_buffer_view* cbs[] { _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);

	_target_size = st_vec2f { float(target->get_width()), float(target->get_height()) };
}

st_bloom_downsample_material::~st_bloom_downsample_material()
{
	_cb = nullptr;
	_cbv = nullptr;
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_bloom_downsample_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_bloom_downsample_cb data;
	data._target_dim =
	{
		_target_size.x,
		_target_size.y,
		1.0f / _target_size.x,
		1.0f / _target_size.y,
	};
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->transition(_texture->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());
}

st_bloom_upsample_material::st_bloom_upsample_material(
	st_render_texture* blur,
	st_render_texture* step,
	st_render_texture* target,
	st_vertex_format* vertex_format,
	st_render_pass* pass) :
	st_material(e_st_render_pass_type::bloom),
	_blur(blur),
	_step(step)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_bloom_upsample_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	st_graphics_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_bloom_upsample);
	desc._blend_desc._target_blend[0]._blend = false;
	desc._depth_stencil_desc._depth_enable = false;
	desc._vertex_format = vertex_format;
	desc._pass = pass;
	desc._render_target_count = 1;
	desc._render_target_formats[0] = target->get_format();

	_pipeline = context->create_graphics_pipeline(desc);

	_resource_table = context->create_resource_table();
	const st_sampler* samplers[] =
	{
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get(),
	};
	const st_texture_view* textures[] =
	{
		_blur->get_resource_view(),
		_step->get_resource_view(),
	};
	context->set_textures(_resource_table.get(), 2, textures, samplers);
	const st_buffer_view* cbs[]{ _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);
}

st_bloom_upsample_material::~st_bloom_upsample_material()
{
	_cb = nullptr;
	_cbv = nullptr;
	_pipeline = nullptr;
	_resource_table = nullptr;
}

void st_bloom_upsample_material::bind(
	class st_graphics_context* context,
	enum e_st_render_pass_type pass_type,
	const struct st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_bloom_upsample_cb data;
	data._source_dim =
	{
		float(_blur->get_width()),
		float(_blur->get_height()),
		1.0f / _blur->get_width(),
		1.0f / _blur->get_height(),
	};
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->transition(_blur->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_step->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());
}
