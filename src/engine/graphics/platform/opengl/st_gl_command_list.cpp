/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_command_list.h>

#if defined(ST_GRAPHICS_API_OPENGL)

st_gl_command_list::st_gl_command_list()
{
}

st_gl_command_list::~st_gl_command_list()
{
}

void st_gl_command_list::begin(st_command_allocator* command_allocator)
{
}

void st_gl_command_list::end()
{
}

void st_gl_command_list::set_pipeline(const st_pipeline* state)
{
}

void st_gl_command_list::set_compute_pipeline(const st_pipeline* state)
{
}

void st_gl_command_list::set_viewport(const st_viewport& viewport)
{
}

void st_gl_command_list::set_scissor(int left, int top, int right, int bottom)
{
}

void st_gl_command_list::set_blend_factor(float r, float g, float b, float a)
{
}

void st_gl_command_list::draw(const st_static_drawcall& drawcall)
{
}

void st_gl_command_list::draw(const st_dynamic_drawcall& drawcall)
{
}

void st_gl_command_list::dispatch(const st_dispatch_args& args)
{
}

void st_gl_command_list::begin_marker(const std::string& marker)
{
}

void st_gl_command_list::end_marker()
{
}

void st_gl_command_list::upload(st_texture* texture, void* data)
{
}

void st_gl_command_list::transition(st_texture* texture, e_st_texture_state new_state)
{
}

void st_gl_command_list::update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count)
{
}

void st_gl_command_list::bind_resources(st_resource_table* table)
{
}

void st_gl_command_list::bind_compute_resources(st_resource_table* table)
{
}

void st_gl_command_list::begin_render_pass(
	st_render_pass* pass,
	st_framebuffer* framebuffer,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
}

void st_gl_command_list::end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer)
{
}

#endif
