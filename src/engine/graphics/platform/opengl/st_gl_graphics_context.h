#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/st_graphics_context.h>

#include <memory>

class st_gl_graphics_context : public st_graphics_context
{
public:

	st_gl_graphics_context(const class st_window* window);
	~st_gl_graphics_context();

	void acquire() override;
	void release() override;

	void set_pipeline(const st_pipeline* state) override;
	void set_compute_pipeline(const st_pipeline* state) override;
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_clear_color(float r, float g, float b, float a) override;
	void set_blend_factor(float r, float g, float b, float a) override {}

	void clear(unsigned int clear_flags) override;
	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Compute.
	void dispatch(const st_dispatch_args& args) override;

	// Swap chain.
	std::unique_ptr<st_swap_chain> create_swap_chain(const st_swap_chain_desc& desc) override;
	st_texture* get_backbuffer(st_swap_chain* swap_chain, uint32_t index) override;
	st_texture_view* get_backbuffer_view(st_swap_chain* swap_chain, uint32_t index) override;
	void acquire_backbuffer(st_swap_chain* swap_chain) override {}

	void begin_loading() override {};
	void end_loading() override {};
	void begin_frame() override {};
	void end_frame() override {};
	void execute() override {};
	void present(st_swap_chain* swap_chain) override;
	void wait_for_idle() override;

	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	std::unique_ptr<st_texture> create_texture(const st_texture_desc& desc) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	void transition(
		st_texture* texture,
		e_st_texture_state new_state) override {};
	std::unique_ptr<st_texture_view> create_texture_view(const st_texture_view_desc& desc) override;

	// Samplers.
	std::unique_ptr<st_sampler> create_sampler(const st_sampler_desc& desc) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(const st_buffer_desc& usage) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(const st_buffer_view_desc& desc) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;
	void set_buffer_name(st_buffer* buffer, std::string name) override;

	// Resource tables.
	std::unique_ptr<st_resource_table> create_resource_table() override;
	std::unique_ptr<st_resource_table> create_resource_table_compute() override;
	void set_constant_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** cbs) override;
	void set_textures(
		st_resource_table* table,
		uint32_t count,
		const st_texture_view** textures,
		const st_sampler** samplers) override;
	void set_buffers(st_resource_table* table, uint32_t count, const st_buffer_view** buffers) override;
	void set_uavs(st_resource_table* table, uint32_t count, const st_texture_view** textures) override;
	void update_textures(st_resource_table* table, uint32_t count, const st_texture_view** views) override;
	void bind_resources(st_resource_table* table) override;
	void bind_compute_resources(st_resource_table* table) override;

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, e_st_shader_type_flags type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_graphics_pipeline(const struct st_graphics_state_desc& desc) override;
	std::unique_ptr<st_pipeline> create_compute_pipeline(const struct st_compute_state_desc& desc) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(const st_render_pass_desc& desc) override;
	void begin_render_pass(
		st_render_pass* pass,
		st_framebuffer* framebuffer,
		const st_clear_value* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer) override;

	// Framebuffers.
	std::unique_ptr<st_framebuffer> create_framebuffer(const st_framebuffer_desc& desc) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::opengl; }
	void get_desc(const st_texture* texture, st_texture_desc* out_desc) override;

private:
	void bind_framebuffer(st_framebuffer* framebuffer);
	void unbind_framebuffer(st_framebuffer* framebuffer);
	void set_depth_state(bool enable, GLenum func);
	void set_cull_state(bool enable, GLenum mode);
	void set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor);
	void set_depth_mask(bool enable);

	const class st_gl_shader* get_bound_shader() const { return _bound_shader; }

private:

	HDC _device_context;
	HGLRC _gl_context;

	// Active draw state.
	float _clear_color[4] = { 0 };
	const class st_gl_shader* _bound_shader = nullptr;
	const struct st_gl_pipeline* _bound_pipeline = nullptr;
};

#endif
