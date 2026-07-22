/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_device.h>

#if defined(ST_GRAPHICS_API_OPENGL)

st_gl_device::st_gl_device()
{
}

st_gl_device::~st_gl_device()
{
}

std::unique_ptr<st_command_queue> st_gl_device::create_command_queue(const st_command_queue_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_command_allocator> st_gl_device::create_command_allocator(const st_command_allocator_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_command_list> st_gl_device::create_command_list(const st_command_list_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_fence> st_gl_device::create_fence(const st_fence_desc& desc)
{
	return nullptr;
}

void st_gl_device::wait(st_fence* fence, uint64_t value)
{
}

std::unique_ptr<st_swap_chain> st_gl_device::create_swap_chain(const st_swap_chain_desc& desc)
{
	return nullptr;
}

void st_gl_device::reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain)
{
}

st_texture* st_gl_device::get_backbuffer(st_swap_chain* swap_chain, uint32_t index)
{
	return nullptr;
}

st_texture_view* st_gl_device::get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index)
{
	return nullptr;
}

uint32_t st_gl_device::get_backbuffer_index(st_swap_chain* swap_chain)
{
	return 0;
}

e_st_swap_chain_status st_gl_device::acquire_backbuffer(st_swap_chain* swap_chain)
{
	return e_st_swap_chain_status::out_of_date;
}

std::unique_ptr<st_texture> st_gl_device::create_texture(const st_texture_desc& desc)
{
	return nullptr;
}

void st_gl_device::set_texture_name(st_texture* texture, std::string name)
{
}

std::unique_ptr<st_texture_view> st_gl_device::create_texture_view(const st_texture_view_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_sampler> st_gl_device::create_sampler(const st_sampler_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_buffer> st_gl_device::create_buffer(const st_buffer_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_buffer_view> st_gl_device::create_buffer_view(const st_buffer_view_desc& desc)
{
	return nullptr;
}

void st_gl_device::map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData)
{
}

void st_gl_device::unmap(st_buffer* buffer, uint32_t subresource, const st_range& range)
{
}

void st_gl_device::set_buffer_name(st_buffer* buffer, std::string name)
{
}

std::unique_ptr<st_resource_table> st_gl_device::create_resource_table()
{
	return nullptr;
}

std::unique_ptr<st_resource_table> st_gl_device::create_resource_table_compute()
{
	return nullptr;
}

void st_gl_device::set_constant_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** cbs)
{
}

void st_gl_device::set_textures(
	st_resource_table* table,
	uint32_t count,
	const st_texture_view** textures,
	const st_sampler** samplers)
{
}

void st_gl_device::set_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** buffers)
{
}

void st_gl_device::set_uavs(st_resource_table* table, uint32_t count, const st_texture_view** textures)
{
}

void st_gl_device::update_textures(st_resource_table* table, uint32_t count, const st_texture_view** views)
{
}

std::unique_ptr<st_shader> st_gl_device::create_shader(const char* filename, e_st_shader_type_flags type)
{
	return nullptr;
}

std::unique_ptr<st_pipeline> st_gl_device::create_graphics_pipeline(const st_graphics_state_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_pipeline> st_gl_device::create_compute_pipeline(const st_compute_state_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_vertex_format> st_gl_device::create_vertex_format(
	const st_vertex_attribute* attributes,
	uint32_t attribute_count)
{
	return nullptr;
}

std::unique_ptr<st_render_pass> st_gl_device::create_render_pass(const st_render_pass_desc& desc)
{
	return nullptr;
}

std::unique_ptr<st_framebuffer> st_gl_device::create_framebuffer(const st_framebuffer_desc& desc)
{
	return nullptr;
}

void st_gl_device::get_desc(const st_texture* texture, st_texture_desc* out_desc)
{
}

#endif
