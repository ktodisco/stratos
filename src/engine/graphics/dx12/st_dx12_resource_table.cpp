/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/dx12/st_dx12_resource_table.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/dx12/st_dx12_constant_buffer.h>
#include <graphics/dx12/st_dx12_render_context.h>
#include <graphics/dx12/st_dx12_texture.h>

st_dx12_resource_table::st_dx12_resource_table()
{
	_cbv_srv_offset = st_dx12_render_context::get()->get_first_cbv_srv_slot();
	_sampler_offset = st_dx12_render_context::get()->get_first_sampler_slot();
}

st_dx12_resource_table::~st_dx12_resource_table()
{
}

void st_dx12_resource_table::add_constant_buffer(class st_dx12_constant_buffer* cb)
{
	st_dx12_render_context::get()->create_constant_buffer_view(
		cb->get_virtual_address(),
		cb->get_size());
	_cbv_count++;
}

void st_dx12_resource_table::add_shader_resource(class st_dx12_texture* sr)
{
	st_dx12_render_context::get()->create_shader_resource_view(
		sr->get_resource(),
		sr->get_format());
	st_dx12_render_context::get()->create_shader_sampler();
}

void st_dx12_resource_table::bind(class st_dx12_render_context* context)
{
	context->set_constant_buffer_table(_cbv_srv_offset);
	context->set_shader_resource_table(_cbv_srv_offset + _cbv_count);
	context->set_sampler_table(_sampler_offset);
}

#endif
