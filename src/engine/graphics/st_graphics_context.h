#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

class st_graphics_context
{
public:
	virtual ~st_graphics_context() {}

	virtual void acquire() = 0;
	virtual void release() = 0;

	virtual void set_pipeline(const st_pipeline* state) = 0;
	virtual void set_viewport(const st_viewport& viewport) = 0;
	virtual void set_scissor(int left, int top, int right, int bottom) = 0;
	virtual void set_clear_color(float r, float g, float b, float a) = 0;
	virtual void set_blend_factor(float r, float g, float b, float a) = 0;

	virtual void set_render_targets(
		uint32_t count,
		const st_texture_view** targets,
		const st_texture_view* depth_stencil) = 0;

	virtual void clear(unsigned int clear_flags) = 0;
	virtual void draw(const struct st_static_drawcall& drawcall) = 0;
	virtual void draw(const struct st_procedural_drawcall& drawcall) = 0;

	// Backbuffer.
	virtual class st_render_texture* get_present_target() const = 0;
	// TODO: These are temporary and a generic solution is needed.
	virtual void transition_backbuffer_to_target() = 0;
	virtual void transition_backbuffer_to_present() = 0;

	virtual void begin_loading() = 0;
	virtual void end_loading() = 0;
	virtual void begin_frame() = 0;
	virtual void end_frame() = 0;
	virtual void swap() = 0;

	virtual void begin_marker(const std::string& marker) = 0;
	virtual void end_marker() = 0;

	// Textures.
	virtual std::unique_ptr<st_texture> create_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		const st_vec4f& clear,
		void* data) = 0;
	virtual void set_texture_meta(st_texture* texture, const char* name) = 0;
	virtual void set_texture_name(st_texture* texture, std::string name) = 0;
	virtual void transition(
		st_texture* texture,
		e_st_texture_state new_state) = 0;
	virtual std::unique_ptr<st_texture_view> create_texture_view(st_texture* texture) = 0;

	// Buffers.
	virtual std::unique_ptr<st_buffer> create_buffer(
		const uint32_t count,
		const size_t element_size,
		const e_st_buffer_usage_flags usage) = 0;
	virtual std::unique_ptr<st_buffer_view> create_buffer_view(st_buffer* buffer) = 0;
	virtual void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) = 0;
	virtual void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) = 0;
	virtual void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) = 0;
	virtual void set_buffer_meta(st_buffer* buffer, std::string name) = 0;

	// Constant buffers.
	virtual void add_constant(
		st_buffer* buffer,
		const std::string& name,
		const e_st_shader_constant_type constant_type) = 0;

	// Resource tables.
	virtual std::unique_ptr<st_resource_table> create_resource_table() = 0;
	virtual void set_constant_buffers(st_resource_table* table, uint32_t count, st_buffer** cbs) = 0;
	virtual void set_textures(st_resource_table* table, uint32_t count, st_texture** textures) = 0;
	virtual void set_buffers(st_resource_table* table, uint32_t count, st_buffer** buffers) = 0;
	virtual void update_textures(st_resource_table* table, uint32_t count, st_texture_view** views) = 0;
	virtual void bind_resource_table(st_resource_table* table) = 0;

	// Shaders.
	virtual std::unique_ptr<st_shader> create_shader(const char* filename, uint8_t type) = 0;

	// Pipelines.
	virtual std::unique_ptr<st_pipeline> create_pipeline(
		const struct st_pipeline_state_desc& desc,
		const struct st_render_pass* render_pass) = 0;

	// Geometry.
	virtual std::unique_ptr<st_vertex_format> create_vertex_format(
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) = 0;

	// Render passes.
	virtual std::unique_ptr<st_render_pass> create_render_pass(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil) = 0;
	virtual void begin_render_pass(
		st_render_pass* pass,
		st_vec4f* clear_values,
		const uint8_t clear_count) = 0;
	virtual void end_render_pass(st_render_pass* pass) = 0;

	// Informational.
	virtual e_st_graphics_api get_api() = 0;

	static std::unique_ptr<st_graphics_context> create(e_st_graphics_api api, const class st_window* window);
	static st_graphics_context* get();

protected:
	static st_graphics_context* _this;
};