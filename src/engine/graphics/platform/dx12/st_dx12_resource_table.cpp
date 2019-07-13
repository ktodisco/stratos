/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_resource_table.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_buffer.h>
#include <graphics/platform/dx12/st_dx12_constant_buffer.h>
#include <graphics/platform/dx12/st_dx12_render_context.h>
#include <graphics/platform/dx12/st_dx12_texture.h>

st_dx12_resource_table::st_dx12_resource_table()
{
}

st_dx12_resource_table::~st_dx12_resource_table()
{
	for (auto& d : _cbvs)
	{
		st_dx12_render_context::get()->destroy_constant_buffer_view(d);
	}

	for (auto& d : _srvs)
	{
		st_dx12_render_context::get()->destroy_shader_resource_view(d);
	}

	for (auto& d : _samplers)
	{
		st_dx12_render_context::get()->destroy_shader_sampler(d);
	}

	for (auto& d : _buffers)
	{
		st_dx12_render_context::get()->destroy_buffer_view(d);
	}
}

void st_dx12_resource_table::add_constant_buffer(st_dx12_constant_buffer* cb)
{
	st_dx12_descriptor cbv = st_dx12_render_context::get()->create_constant_buffer_view(
		cb->get_virtual_address(),
		cb->get_size());
	_cbvs.push_back(cbv);
}

void st_dx12_resource_table::add_texture_resource(st_dx12_texture* sr)
{
	// TODO: Depending on whether we want depth or stencil, this needs to be flexible.
	e_st_format format = sr->get_format();
	if (format == st_format_d24_unorm_s8_uint)
	{
		format = st_format_r24_unorm_x8_typeless;
	}

	st_dx12_descriptor srv = st_dx12_render_context::get()->create_shader_resource_view(
		sr->get_resource(),
		format,
		sr->get_levels());
	st_dx12_descriptor sampler = st_dx12_render_context::get()->create_shader_sampler();

	_srvs.push_back(srv);
	_samplers.push_back(sampler);
}

void st_dx12_resource_table::add_buffer_resource(st_dx12_buffer* br)
{
	// TODO: I'm thinking it may be better, below, to replace set_constant_buffer_table,
	// set_shader_resource_table, and set_sampler_table with a more generic
	// set_graphics_resource_table which takes an enum describing which resource table
	// to set.  We could define the targets as more generic entries like constant buffers,
	// textures, buffers, and samplers. ... Which is pretty much what we have anyway,
	// I realize...  I think this means that the root signature just needs to be tweaked
	// to accommodate more flexibility.
	st_dx12_descriptor srv = st_dx12_render_context::get()->create_buffer_view(
		br->get_resource(),
		br->get_count(),
		br->get_element_size());
	_buffers.push_back(srv);
}

void st_dx12_resource_table::bind(st_dx12_render_context* context)
{
	// TODO: Find a way to eliminate the conditionals here.
	if (_cbvs.size() > 0) { context->set_constant_buffer_table(_cbvs[0]); }
	if (_srvs.size() > 0) { context->set_shader_resource_table(_srvs[0]); }
	if (_samplers.size() > 0) { context->set_sampler_table(_samplers[0]); }
	if (_buffers.size() > 0) { context->set_buffer_table(_buffers[0]); }
}

#endif
