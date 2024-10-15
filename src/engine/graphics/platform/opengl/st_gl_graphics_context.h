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
	void set_viewport(const st_viewport& viewport) override;
	void set_scissor(int left, int top, int right, int bottom) override;
	void set_clear_color(float r, float g, float b, float a) override;
	void set_blend_factor(float r, float g, float b, float a) override {}

	void set_render_targets(
		uint32_t count,
		const st_texture_view** targets,
		const st_texture_view* depth_stencil) override;

	void clear(unsigned int clear_flags) override;
	void draw(const struct st_static_drawcall& drawcall) override;
	void draw(const struct st_dynamic_drawcall& drawcall) override;

	// Backbuffer.
	class st_render_texture* get_present_target() const override { return _present_target.get(); };
	// TODO: These are temporary and a generic solution is needed.
	void transition_backbuffer_to_target() override {};
	void transition_backbuffer_to_present() override {};

	void begin_loading() override {};
	void end_loading() override {};
	void begin_frame() override {};
	void end_frame() override {};
	void swap() override;

	void begin_marker(const std::string& marker) override;
	void end_marker() override;

	// Textures.
	std::unique_ptr<st_texture> create_texture(
		uint32_t width,
		uint32_t height,
		uint32_t levels,
		e_st_format format,
		e_st_texture_usage_flags usage,
		e_st_texture_state initial_state,
		const st_vec4f& clear,
		void* data) override;
	void set_texture_meta(st_texture* texture, const char* name) override;
	void set_texture_name(st_texture* texture, std::string name) override;
	void transition(
		st_texture* texture,
		e_st_texture_state new_state) override {};
	std::unique_ptr<st_texture_view> create_texture_view(st_texture* texture) override;

	// Buffers.
	std::unique_ptr<st_buffer> create_buffer(
		const uint32_t count,
		const size_t element_size,
		const e_st_buffer_usage_flags usage) override;
	std::unique_ptr<st_buffer_view> create_buffer_view(st_buffer* buffer) override;
	void map(st_buffer* buffer, uint32_t subresource, const st_range& range, void** outData) override;
	void unmap(st_buffer* buffer, uint32_t subresource, const st_range& range) override;
	void update_buffer(st_buffer* buffer, void* data, const uint32_t offset, const uint32_t count) override;
	void set_buffer_meta(st_buffer* buffer, std::string name) override;

	// Constant buffers.
	void add_constant(
		st_buffer* buffer,
		const std::string& name,
		const e_st_shader_constant_type constant_type) override;

	// Resource tables.
	std::unique_ptr<st_resource_table> create_resource_table() override;
	void set_constant_buffers(st_resource_table* table, uint32_t count, st_buffer** cbs) override;
	void set_textures(st_resource_table* table, uint32_t count, st_texture** textures) override;
	void set_buffers(st_resource_table* table, uint32_t count, st_buffer** buffers) override;
	void update_textures(st_resource_table* table, uint32_t count, st_texture_view** views) override;
	void bind_resource_table(st_resource_table* table) override;

	// Shaders.
	std::unique_ptr<st_shader> create_shader(const char* filename, uint8_t type) override;

	// Pipelines.
	std::unique_ptr<st_pipeline> create_pipeline(
		const struct st_pipeline_state_desc& desc,
		const struct st_render_pass* render_pass) override;

	// Geometry.
	std::unique_ptr<st_vertex_format> create_vertex_format(
		const struct st_vertex_attribute* attributes,
		uint32_t attribute_count) override;

	// Render passes.
	std::unique_ptr<st_render_pass> create_render_pass(
		uint32_t count,
		struct st_target_desc* targets,
		struct st_target_desc* depth_stencil) override;
	void begin_render_pass(
		st_render_pass* pass,
		const st_clear_value* clear_values,
		const uint8_t clear_count) override;
	void end_render_pass(st_render_pass* pass) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::opengl; }

private:
	void set_depth_state(bool enable, GLenum func);
	void set_cull_state(bool enable, GLenum mode);
	void set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor);
	void set_depth_mask(bool enable);

	const class st_gl_shader* get_bound_shader() const { return _bound_shader; }

private:
	std::unique_ptr<class st_render_texture> _present_target;
	std::unique_ptr<class st_gl_framebuffer> _present_framebuffer;

	HDC _device_context;
	HGLRC _gl_context;

	// Active draw state.
	float _clear_color[4] = { 0 };
	const class st_gl_shader* _bound_shader = nullptr;
	const struct st_gl_pipeline* _bound_pipeline = nullptr;
};

#endif
