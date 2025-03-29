#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <vector>

class st_graphics_context
{
public:
	virtual ~st_graphics_context() {}

	virtual void acquire() = 0;
	virtual void release() = 0;

	virtual void set_pipeline(const st_pipeline* state) = 0;
	virtual void set_compute_pipeline(const st_pipeline* state) = 0;
	virtual void set_viewport(const st_viewport& viewport) = 0;
	virtual void set_scissor(int left, int top, int right, int bottom) = 0;
	virtual void set_blend_factor(float r, float g, float b, float a) = 0;

	virtual void draw(const struct st_static_drawcall& drawcall) = 0;
	virtual void draw(const struct st_dynamic_drawcall& drawcall) = 0;

	// Compute.
	virtual void dispatch(const st_dispatch_args& args) = 0;

	// Swap chain.
	virtual std::unique_ptr<st_swap_chain> create_swap_chain(const st_swap_chain_desc& desc) = 0;
	virtual void reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain) = 0;
	virtual st_texture* get_backbuffer(st_swap_chain* swap_chain, uint32_t index) = 0;
	virtual st_texture_view* get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index) = 0;
	virtual e_st_swap_chain_status acquire_backbuffer(st_swap_chain* swap_chain) = 0;

	virtual void begin_loading() = 0;
	virtual void end_loading() = 0;
	virtual void begin_frame() = 0;
	virtual void end_frame() = 0;
	virtual void execute() = 0;
	virtual void present(st_swap_chain* swap_chain) = 0;
	virtual void wait_for_idle() = 0;

	virtual void begin_marker(const std::string& marker) = 0;
	virtual void end_marker() = 0;

	// Textures.
	virtual std::unique_ptr<st_texture> create_texture(const st_texture_desc& desc) = 0;
	virtual void set_texture_name(st_texture* texture, std::string name) = 0;
	virtual void transition(
		st_texture* texture,
		e_st_texture_state new_state) = 0;
	virtual std::unique_ptr<st_texture_view> create_texture_view(const st_texture_view_desc& desc) = 0;

	// Samplers.
	virtual std::unique_ptr<st_sampler> create_sampler(const st_sampler_desc& desc) = 0;

	// Buffers.
	virtual std::unique_ptr<st_buffer> create_buffer(const st_buffer_desc& desc) = 0;
	virtual std::unique_ptr<st_buffer_view> create_buffer_view(const st_buffer_view_desc& desc) = 0;
	virtual void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) = 0;
	virtual void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) = 0;
	virtual void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) = 0;
	virtual void set_buffer_name(st_buffer* buffer, std::string name) = 0;

	// Resource tables.
	// TODO: This would take a root signature object. In the vk backend it would pull the descriptor layouts
	// from that root signature.
	// I especially don't like this because there isn't any barrier to forgetting to use one or the other.
	// The new API would look like a create and a generic set_resources which has overloads for texture
	// and buffer views, and each takes the root signature and the slot the resources are going to.
	virtual std::unique_ptr<st_resource_table> create_resource_table() = 0;
	virtual std::unique_ptr<st_resource_table> create_resource_table_compute() = 0;
	virtual void set_constant_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** cbs) = 0;
	virtual void set_textures(
		st_resource_table* table,
		uint32_t count,
		const st_texture_view** textures,
		const st_sampler** samplers) = 0;
	virtual void set_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** buffers) = 0;
	virtual void set_uavs(st_resource_table* table, uint32_t count, const st_texture_view** textures) = 0;
	virtual void update_textures(st_resource_table* table, uint32_t count, const st_texture_view** views) = 0;
	virtual void bind_resources(st_resource_table* table) = 0;
	virtual void bind_compute_resources(st_resource_table* table) = 0;

	// Shaders.
	virtual std::unique_ptr<st_shader> create_shader(const char* filename, e_st_shader_type_flags type) = 0;

	// Pipelines.
	virtual std::unique_ptr<st_pipeline> create_graphics_pipeline(const struct st_graphics_state_desc& desc) = 0;
	virtual std::unique_ptr<st_pipeline> create_compute_pipeline(const struct st_compute_state_desc& desc) = 0;

	// Geometry.
	virtual std::unique_ptr<st_vertex_format> create_vertex_format(
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) = 0;

	// Render passes.
	virtual std::unique_ptr<st_render_pass> create_render_pass(const st_render_pass_desc& desc) = 0;
	virtual void begin_render_pass(
		st_render_pass* pass,
		st_framebuffer* framebuffer,
		const st_clear_value* clear_values,
		const uint8_t clear_count) = 0;
	virtual void end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer) = 0;

	// Framebuffers.
	virtual std::unique_ptr<st_framebuffer> create_framebuffer(const st_framebuffer_desc& desc) = 0;

	// Informational.
	virtual e_st_graphics_api get_api() = 0;
	virtual void get_desc(const st_texture* texture, st_texture_desc* out_desc) = 0;
	virtual void get_supported_formats(
		const class st_window* window,
		std::vector<e_st_format>& formats) = 0;

	uint32_t get_frame_index() const { return _frame_index; }

	static std::unique_ptr<st_graphics_context> create(e_st_graphics_api api, const class st_window* window);
	static st_graphics_context* get();

protected:
	uint32_t _frame_index = 0;
	static st_graphics_context* _this;
};