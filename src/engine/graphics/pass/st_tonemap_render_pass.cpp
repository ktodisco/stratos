/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_tonemap_render_pass.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>

#include <graphics/geometry/st_geometry.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

struct st_tonemap_constants
{
	float _is_hdr;

	float pad0;
	float pad1;
	float pad2;
};

st_tonemap_render_pass::st_tonemap_render_pass(
	st_render_texture* source_buffer,
	st_render_texture* bloom_buffer,
	st_render_texture* target_buffer) :
	_source(source_buffer),
	_bloom(bloom_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_tonemap_constants);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);

		st_buffer_view_desc view_desc;
		view_desc._buffer = _cb.get();
		view_desc._usage = e_st_view_usage::shader_resource;
		_cbv = context->create_buffer_view(view_desc);
	}

	{
		st_attachment_desc attachment = { target_buffer->get_format(), e_st_load_op::dont_care, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._viewport = { 0.0f, 0.0f, float(target_buffer->get_width()), float(target_buffer->get_height()), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { target_buffer->get_texture(), target_buffer->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_tonemap);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._vertex_format = _vertex_format.get();
		desc._pass = _pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target_buffer->get_format();

		_pipeline = context->create_graphics_pipeline(desc);
	}

	_resource_table = context->create_resource_table();
	const st_texture_view* textures[] =
	{
		_source->get_resource_view(),
		_bloom->get_resource_view()
	};
	const st_sampler* samplers[] =
	{
		_global_resources->_trilinear_clamp_sampler.get(),
		_global_resources->_trilinear_clamp_sampler.get()
	};
	context->set_textures(_resource_table.get(), 2, textures, samplers);
	const st_buffer_view* buffers[] = { _cbv.get() };
	context->set_constant_buffers(_resource_table.get(), 1, buffers);
}

st_tonemap_render_pass::~st_tonemap_render_pass()
{
}

void st_tonemap_render_pass::render(
	st_graphics_context* context,
	const st_frame_params* params)
{
	st_render_marker marker(context, "st_tonemap_render_pass::render");

	st_mat4f identity;
	identity.make_identity();

	st_tonemap_constants data;
	data._is_hdr = params->_color_space == st_color_space_st2084;
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->set_scissor(0, 0, params->_width, params->_height);

	context->set_pipeline(_pipeline.get());

	context->transition(_source->get_texture(), st_texture_state_pixel_shader_read);
	context->transition(_bloom->get_texture(), st_texture_state_pixel_shader_read);
	context->bind_resources(_resource_table.get());

	st_clear_value clears[] =
	{
		st_vec4f { 0.0f, 0.0f, 0.0f, 1.0f },
	};

	context->begin_render_pass(_pass.get(), _framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffer.get());
}
