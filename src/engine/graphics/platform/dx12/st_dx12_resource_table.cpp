/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_resource_table.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_constant_buffer.h>
#include <graphics/platform/dx12/st_dx12_render_context.h>
#include <graphics/platform/dx12/st_dx12_texture.h>

st_dx12_resource_table::st_dx12_resource_table()
{
}

st_dx12_resource_table::~st_dx12_resource_table()
{
	for (auto itr = _cbvs.begin(); itr != _cbvs.end(); ++itr)
	{
		st_dx12_render_context::get()->destroy_constant_buffer_view(*itr);
	}

	for (auto itr = _srvs.begin(); itr != _srvs.end(); ++itr)
	{
		st_dx12_render_context::get()->destroy_shader_resource_view(*itr);
	}

	for (auto itr = _samplers.begin(); itr != _samplers.end(); ++itr)
	{
		st_dx12_render_context::get()->destroy_shader_sampler(*itr);
	}
}

void st_dx12_resource_table::add_constant_buffer(class st_dx12_constant_buffer* cb)
{
	st_dx12_descriptor cbv = st_dx12_render_context::get()->create_constant_buffer_view(
		cb->get_virtual_address(),
		cb->get_size());
	_cbvs.push_back(cbv);
}

void st_dx12_resource_table::add_shader_resource(class st_dx12_texture* sr)
{
	// TODO: Depending on whether we want depth or stencil, this needs to be flexible.
	e_st_texture_format format = sr->get_format();
	if (format == st_texture_format_d24_unorm_s8_uint)
	{
		format = st_texture_format_r24_unorm_x8_typeless;
	}

	st_dx12_descriptor srv = st_dx12_render_context::get()->create_shader_resource_view(
		sr->get_resource(),
		format);
	st_dx12_descriptor sampler = st_dx12_render_context::get()->create_shader_sampler();

	_srvs.push_back(srv);
	_samplers.push_back(sampler);
}

void st_dx12_resource_table::bind(class st_dx12_render_context* context)
{
	// TODO: Find a way to eliminate the ternary operators here.
	context->set_constant_buffer_table(_cbvs.size() > 0 ? _cbvs[0] : 0);
	context->set_shader_resource_table(_srvs.size() > 0 ? _srvs[0] : 0);
	context->set_sampler_table(_samplers.size() > 0 ? _samplers[0] : 0);
}

#endif
