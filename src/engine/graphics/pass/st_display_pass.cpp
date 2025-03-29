/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_display_pass.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>

#include <graphics/st_graphics_context.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>

struct st_display_constants
{
	float _is_hdr;

	float pad0;
	float pad1;
	float pad2;
};

st_display_pass::st_display_pass(st_render_texture* source, st_swap_chain* swap_chain) :
	_source(source)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_display_constants);
		desc._usage = e_st_buffer_usage::uniform;
		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		_cbv = context->create_buffer_view(desc);
	}

	st_texture_desc target_desc;
	context->get_desc(context->get_backbuffer(swap_chain, 0), &target_desc);

	{
		st_attachment_desc attachments[] =
		{
			{ target_desc._format, e_st_load_op::load, e_st_store_op::store }
		};
		st_render_pass_desc desc;
		desc._attachments = attachments;
		desc._attachment_count = std::size(attachments);
		desc._viewport = { 0.0f, 0.0f, float(target_desc._width), float(target_desc._height), 0.0f, 1.0f };

		_pass = context->create_render_pass(desc);
	}

	for (uint32_t i = 0; i < std::size(_framebuffers); ++i)
	{
		st_target_desc t_desc = {
			context->get_backbuffer(swap_chain, i),
			context->get_backbuffer_view(swap_chain, i)
		};
		st_framebuffer_desc desc;
		desc._pass = _pass.get();
		desc._targets = &t_desc;
		desc._target_count = 1;

		_framebuffers[i] = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_display);
		desc._vertex_format = _vertex_format.get();
		desc._pass = _pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target_desc._format;
		desc._blend_desc._target_blend[0]._blend = false;
		_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		const st_texture_view* textures[] = { _source->get_resource_view() };
		const st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
		const st_buffer_view* cbs[] = { _cbv.get() };
		_resources = context->create_resource_table();
		context->set_constant_buffers(_resources.get(), _countof(cbs), cbs);
		context->set_textures(_resources.get(), _countof(textures), textures, samplers);
	}
}

st_display_pass::~st_display_pass()
{
	_pass = nullptr;
	_resources = nullptr;
	_pipeline = nullptr;
	_cbv = nullptr;
	_cb = nullptr;
}

void st_display_pass::render(class st_graphics_context* context, const st_frame_params* params)
{
	st_render_marker marker(context, __FUNCTION__);

	st_mat4f identity;
	identity.make_identity();

	context->set_scissor(0, 0, params->_width, params->_height);

	st_display_constants constants;
	constants._is_hdr = (params->_color_space == st_color_space_st2084);
	context->update_buffer(_cb.get(), &constants, 0, 1);

	context->transition(_source->get_texture(), st_texture_state_pixel_shader_read);

	context->set_pipeline(_pipeline.get());
	context->bind_resources(_resources.get());

	context->begin_render_pass(_pass.get(), _framebuffers[params->_frame_index].get(), nullptr, 0);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_pass.get(), _framebuffers[params->_frame_index].get());
}