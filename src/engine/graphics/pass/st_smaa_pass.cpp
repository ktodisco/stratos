/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_smaa_pass.h>

#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>

#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_marker.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

struct st_smaa_constants
{
	st_vec4f _dimensions;
};

st_smaa_pass::st_smaa_pass(
	st_render_texture* source_buffer,
	st_render_texture* stencil_buffer,
	st_swap_chain* swap_chain)
	: _stencil_buffer(stencil_buffer)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_smaa_constants);
		desc._usage = e_st_buffer_usage::uniform;

		_cb = context->create_buffer(desc);
	}

	{
		st_buffer_view_desc desc;
		desc._buffer = _cb.get();
		desc._element_count = 1;
		desc._usage = e_st_view_usage::shader_resource;

		_cbv = context->create_buffer_view(desc);
	}

	_create_edges_pass(context, source_buffer);
	_create_weights_pass(context);
	_create_blend_pass(context, source_buffer, swap_chain);
}

st_smaa_pass::~st_smaa_pass()
{
}

void st_smaa_pass::render(class st_graphics_context* context, const struct st_frame_params* params)
{
	st_render_marker marker(context, __FUNCTION__);

	st_smaa_constants data;
	data._dimensions = st_vec4f {
		float(_edges_target->get_width()),
		float(_edges_target->get_height()),
		1.0f / _edges_target->get_width(),
		1.0f / _edges_target->get_height()
	};
	context->update_buffer(_cb.get(), &data, 0, 1);

	context->set_scissor(0, 0, params->_width, params->_height);

	_render_edges_pass(context, params);
	_render_weights_pass(context, params);
	_render_blend_pass(context, params);
}

void st_smaa_pass::_create_edges_pass(st_graphics_context* context, st_render_texture* source_buffer)
{
	_edges_target = std::make_unique<st_render_texture>(
		context,
		source_buffer->get_width(),
		source_buffer->get_height(),
		st_format_r16g16_float,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		"SMAA Edges");

	{
		st_attachment_desc attachment = { _edges_target->get_format(), e_st_load_op::clear, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._depth_attachment = { _stencil_buffer->get_format(), e_st_load_op::clear, e_st_store_op::store };
		desc._viewport = { 0.0f, 0.0f, float(_edges_target->get_width()), float(_edges_target->get_height()), 0.0f, 1.0f };

		_edges_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { _edges_target->get_texture(), _edges_target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _edges_pass.get();
		desc._targets = &target;
		desc._target_count = 1;
		desc._depth_target = { _stencil_buffer->get_texture(), _stencil_buffer->get_target_view() };

		_edges_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_smaa_edges);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._depth_stencil_desc._stencil_enable = true;
		desc._depth_stencil_desc._front_stencil._stencil_func = st_compare_func_greater;
		desc._depth_stencil_desc._front_stencil._stencil_pass_op = st_stencil_op_replace;
		desc._depth_stencil_desc._stencil_write_mask = 0xff;
		desc._depth_stencil_desc._stencil_read_mask = 0xff;
		desc._depth_stencil_desc._stencil_ref = 0x01;
		desc._vertex_format = _vertex_format.get();
		desc._pass = _edges_pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = _edges_target->get_format();
		desc._depth_stencil_format = _stencil_buffer->get_format();

		_edges_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		_edges_resources = context->create_resource_table();
		const st_buffer_view* cbv = _cbv.get();
		const st_texture_view* srv = source_buffer->get_resource_view();
		const st_sampler* sampler = _global_resources->_point_clamp_sampler.get();
		context->set_constant_buffers(_edges_resources.get(), 1, &cbv);
		context->set_textures(_edges_resources.get(), 1, &srv, &sampler);
	}
}

void st_smaa_pass::_create_weights_pass(st_graphics_context* context)
{
	_weights_target = std::make_unique<st_render_texture>(
		context,
		_edges_target->get_width(),
		_edges_target->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::sampled,
		st_texture_state_pixel_shader_read,
		st_vec4f { 0.0f, 0.0f, 0.0f, 0.0f },
		"SMAA Weights");

	_area_tex = st_texture_loader::load("data/textures/smaa/AreaTex.tga");
	_search_tex = st_texture_loader::load("data/textures/smaa/SearchTex.dds");

	{
		st_texture_desc area_desc;
		context->get_desc(_area_tex.get(), &area_desc);

		st_texture_view_desc desc;
		desc._texture = _area_tex.get();
		desc._format = area_desc._format;
		desc._mips = 1;
		desc._usage = e_st_view_usage::shader_resource;

		_area_tex_view = context->create_texture_view(desc);
	}

	{
		st_texture_desc search_desc;
		context->get_desc(_search_tex.get(), &search_desc);

		st_texture_view_desc desc;
		desc._texture = _search_tex.get();
		desc._format = search_desc._format;
		desc._mips = 1;
		desc._usage = e_st_view_usage::shader_resource;

		_search_tex_view = context->create_texture_view(desc);
	}

	{
		st_attachment_desc attachment = { _weights_target->get_format(), e_st_load_op::clear, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._depth_attachment = { _stencil_buffer->get_format(), e_st_load_op::load, e_st_store_op::dont_care };
		desc._viewport = { 0.0f, 0.0f, float(_weights_target->get_width()), float(_weights_target->get_height()), 0.0f, 1.0f };

		_weights_pass = context->create_render_pass(desc);
	}

	{
		st_target_desc target = { _weights_target->get_texture(), _weights_target->get_target_view() };
		st_framebuffer_desc desc;
		desc._pass = _weights_pass.get();
		desc._targets = &target;
		desc._target_count = 1;
		desc._depth_target = { _stencil_buffer->get_texture(), _stencil_buffer->get_target_view() };

		_weights_framebuffer = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_smaa_weights);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._depth_stencil_desc._stencil_enable = true;
		desc._depth_stencil_desc._stencil_read_mask = 0xff;
		desc._depth_stencil_desc._stencil_ref = 0x01;
		desc._depth_stencil_desc._front_stencil._stencil_func = st_compare_func_equal;
		desc._vertex_format = _vertex_format.get();
		desc._pass = _weights_pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = _weights_target->get_format();
		desc._depth_stencil_format = _stencil_buffer->get_format();

		_weights_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		_weights_resources = context->create_resource_table();
		const st_buffer_view* cbv = _cbv.get();
		const st_texture_view* srvs[] = {
			_edges_target->get_resource_view(),
			_area_tex_view.get(),
			_search_tex_view.get()
		};
		const st_sampler* samplers[] = {
			_global_resources->_trilinear_clamp_sampler.get(),
			_global_resources->_point_clamp_sampler.get(),
			_global_resources->_trilinear_clamp_sampler.get()
		};
		context->set_constant_buffers(_weights_resources.get(), 1, &cbv);
		context->set_textures(_weights_resources.get(), std::size(srvs), srvs, samplers);
	}
}

void st_smaa_pass::_create_blend_pass(
	st_graphics_context* context,
	st_render_texture* source_buffer,
	st_swap_chain* swap_chain)
{
	st_texture_desc target_desc;
	context->get_desc(context->get_backbuffer(swap_chain, 0), &target_desc);

	{
		st_attachment_desc attachment = { target_desc._format, e_st_load_op::dont_care, e_st_store_op::store };
		st_render_pass_desc desc;
		desc._attachments = &attachment;
		desc._attachment_count = 1;
		desc._viewport = { 0.0f, 0.0f, float(target_desc._width), float(target_desc._height), 0.0f, 1.0f };

		_blend_pass = context->create_render_pass(desc);
	}

	for (uint32_t i = 0; i < std::size(_blend_framebuffers); ++i)
	{
		st_target_desc target = {
			context->get_backbuffer(swap_chain, i),
			context->get_backbuffer_view(swap_chain, i)
		};
		st_framebuffer_desc desc;
		desc._pass = _blend_pass.get();
		desc._targets = &target;
		desc._target_count = 1;

		_blend_framebuffers[i] = context->create_framebuffer(desc);
	}

	{
		st_graphics_state_desc desc;
		desc._shader = st_shader_manager::get()->get_shader(st_shader_smaa_blend);
		desc._blend_desc._target_blend[0]._blend = false;
		desc._depth_stencil_desc._depth_enable = false;
		desc._vertex_format = _vertex_format.get();
		desc._pass = _blend_pass.get();
		desc._render_target_count = 1;
		desc._render_target_formats[0] = target_desc._format;

		_blend_pipeline = context->create_graphics_pipeline(desc);
	}

	{
		_blend_resources = context->create_resource_table();
		const st_buffer_view* cbv = _cbv.get();
		const st_texture_view* srvs[] = {
			source_buffer->get_resource_view(),
			_weights_target->get_resource_view()
		};
		const st_sampler* samplers[] = {
			_global_resources->_trilinear_clamp_sampler.get(),
			_global_resources->_point_clamp_sampler.get()
		};
		context->set_constant_buffers(_blend_resources.get(), 1, &cbv);
		context->set_textures(_blend_resources.get(), std::size(srvs), srvs, samplers);
	}
}

void st_smaa_pass::_render_edges_pass(class st_graphics_context* context, const struct st_frame_params* params)
{
	st_render_marker marker(context, "edges");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline(_edges_pipeline.get());
	context->bind_resources(_edges_resources.get());

	st_clear_value clears[] =
	{
		_edges_target->get_clear_value(),
		_stencil_buffer->get_clear_value()
	};

	context->begin_render_pass(_edges_pass.get(), _edges_framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_edges_pass.get(), _edges_framebuffer.get());
}

void st_smaa_pass::_render_weights_pass(class st_graphics_context* context, const struct st_frame_params* params)
{
	st_render_marker marker(context, "weights");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline(_weights_pipeline.get());
	context->bind_resources(_weights_resources.get());

	st_clear_value clears[] =
	{
		_weights_target->get_clear_value(),
		_stencil_buffer->get_clear_value()
	};

	context->begin_render_pass(_weights_pass.get(), _weights_framebuffer.get(), clears, std::size(clears));

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_weights_pass.get(), _weights_framebuffer.get());
}

void st_smaa_pass::_render_blend_pass(class st_graphics_context* context, const struct st_frame_params* params)
{
	st_render_marker marker(context, "blend");

	st_mat4f identity;
	identity.make_identity();

	context->set_pipeline(_blend_pipeline.get());
	context->bind_resources(_blend_resources.get());

	context->begin_render_pass(_blend_pass.get(), _blend_framebuffers[params->_frame_index].get(), nullptr, 0);

	st_static_drawcall draw_call;
	draw_call._name = "fullscreen_quad";
	draw_call._transform = identity;
	_fullscreen_quad->draw(draw_call);
	draw_call._draw_mode = st_primitive_topology_triangles;

	context->draw(draw_call);

	context->end_render_pass(_blend_pass.get(), _blend_framebuffers[params->_frame_index].get());
}