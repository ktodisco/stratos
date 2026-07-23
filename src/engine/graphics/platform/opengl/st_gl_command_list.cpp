/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_command_list.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_conversion.h>
#include <graphics/platform/opengl/st_gl_device.h>
#include <graphics/platform/opengl/st_gl_shader.h>

#include <graphics/st_drawcall.h>
#include <graphics/geometry/st_vertex_attribute.h>

#include <cassert>

st_gl_command_list::st_gl_command_list(st_gl_device* device)
	: _device(device)
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

void st_gl_command_list::set_pipeline(const st_pipeline* state_)
{
	const st_gl_pipeline* state = static_cast<const st_gl_pipeline*>(state_);

	_bound_pipeline = state;

	const st_graphics_state_desc& state_desc = state->_graphics_desc;
	_bound_shader = static_cast<const st_gl_shader*>(state_desc._shader);
	glUseProgram(_bound_shader->_handle);

	set_depth_state(
		state_desc._depth_stencil_desc._depth_enable,
		convert_compare_func(state_desc._depth_stencil_desc._depth_compare));

	set_stencil_state(state_desc._depth_stencil_desc);

	set_depth_mask(
		state_desc._depth_stencil_desc._depth_mask != st_depth_write_mask_zero);

	set_blend_state(
		state_desc._blend_desc._target_blend[0]._blend,
		convert_blend(state_desc._blend_desc._target_blend[0]._src_blend),
		convert_blend(state_desc._blend_desc._target_blend[0]._dst_blend));

	set_cull_state(
		state_desc._rasterizer_desc._cull_mode != st_cull_mode_none,
		convert_cull_mode(state_desc._rasterizer_desc._cull_mode));
}

void st_gl_command_list::set_compute_pipeline(const st_pipeline* state_)
{
	const st_gl_pipeline* state = static_cast<const st_gl_pipeline*>(state_);

	_bound_pipeline = state;

	const st_compute_state_desc& state_desc = state->_compute_desc;
	_bound_shader = static_cast<const st_gl_shader*>(state_desc._shader);
	glUseProgram(_bound_shader->_handle);
}

void st_gl_command_list::set_viewport(const st_viewport& viewport)
{
	glViewport(
		GLint(viewport._x),
		GLint(viewport._y),
		GLsizei(viewport._width),
		GLsizei(viewport._height));
}

void st_gl_command_list::set_scissor(int left, int top, int right, int bottom)
{
	glScissor(left, top, right - left, bottom - top);
}

void st_gl_command_list::set_blend_factor(float r, float g, float b, float a)
{
}

void st_gl_command_list::draw(const st_static_drawcall& drawcall)
{
	const st_gl_buffer* vertex = static_cast<const st_gl_buffer*>(drawcall._vertex_buffer);
	const st_gl_buffer* index = static_cast<const st_gl_buffer*>(drawcall._index_buffer);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertex->_buffer);

	const st_gl_vertex_format* format = static_cast<const st_gl_vertex_format*>(_bound_pipeline->_graphics_desc._vertex_format);

	size_t offset = 0;
	for (uint32_t itr = 0; itr < format->_attributes.size(); ++itr)
	{
		const st_vertex_attribute* attr = &format->_attributes[itr];
		GLint components = 0;
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;

		switch (attr->_format)
		{
		case st_format_r8g8b8a8_unorm:
			type = GL_UNSIGNED_BYTE;
			normalized = GL_TRUE;
			break;
		default:
			type = GL_FLOAT;
			break;
		}

		switch (attr->_format)
		{
		case st_format_r32g32b32a32_float:
		case st_format_r32g32b32a32_uint:
		case st_format_r8g8b8a8_unorm:
			components = 4;
			break;
		case st_format_r32g32b32_float:
			components = 3;
			break;
		case st_format_r32g32_float:
			components = 2;
			break;
		default:
			assert(false);
			break;
		}

		// HACK: We need the special case for joint indices.
		if (attr->_type == st_vertex_attribute_joints)
		{
			glVertexAttribIPointer(
				attr->_unit,
				components,
				type,
				format->_vertex_size,
				(GLvoid*)offset);
		}
		else
		{
			glVertexAttribPointer(
				attr->_unit,
				components,
				type,
				normalized,
				format->_vertex_size,
				(GLvoid*)offset);
		}

		glEnableVertexAttribArray(attr->_unit);

		offset += bytes_per_pixel(attr->_format);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->_buffer);

	size_t index_offset = static_cast<GLint>(drawcall._index_offset) * sizeof(GLushort);
	GLint base_vertex = static_cast<GLint>(drawcall._vertex_offset);
	glDrawElementsBaseVertex(convert_topology(drawcall._draw_mode), drawcall._index_count, GL_UNSIGNED_SHORT, (void*)index_offset, base_vertex);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &vao);
}

void st_gl_command_list::draw(const st_dynamic_drawcall& drawcall)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint pos;
	glGenBuffers(1, &pos);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec3f) * drawcall._positions.size(), drawcall._positions.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLuint color;
	glGenBuffers(1, &color);
	glBindBuffer(GL_ARRAY_BUFFER, color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec3f) * drawcall._colors.size(), drawcall._colors.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	GLuint indices;
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * drawcall._indices.size(), drawcall._indices.data(), GL_STREAM_DRAW);

	glDrawElements(convert_topology(drawcall._draw_mode), (GLsizei)drawcall._indices.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &indices);
	glDeleteBuffers(1, &color);
	glDeleteBuffers(1, &pos);
	glDeleteVertexArrays(1, &vao);
	glBindVertexArray(0);
}

void st_gl_command_list::dispatch(const st_dispatch_args& args)
{
	glDispatchCompute(args.group_count_x, args.group_count_y, args.group_count_z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void st_gl_command_list::begin_marker(const std::string& marker)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, marker.length(), marker.c_str());
}

void st_gl_command_list::end_marker()
{
	glPopDebugGroup();
}

void st_gl_command_list::upload(st_texture* texture_, void* data)
{
	st_gl_texture* texture = static_cast<st_gl_texture*>(texture_);

	glBindTexture(GL_TEXTURE_2D, texture->_handle);

	if (is_compressed(texture->_format))
	{
		uint8_t* bits = reinterpret_cast<uint8_t*>(data);

		for (uint32_t mip = 0; mip < texture->_levels; ++mip)
		{
			uint32_t level_width = texture->_width >> mip;
			uint32_t level_height = texture->_height >> mip;

			size_t row_bytes;
			size_t num_rows;
			get_surface_info(level_width, level_height, texture->_format, nullptr, &row_bytes, &num_rows);

			glCompressedTexSubImage2D(
				GL_TEXTURE_2D,
				mip,
				0,
				0,
				level_width,
				level_height,
				convert_format(texture->_format),
				row_bytes * num_rows,
				bits);

			bits += row_bytes * num_rows;
		}
	}
	else
	{
		GLenum pixel_format;
		GLenum type;
		get_pixel_format_and_type(texture->_format, pixel_format, type);

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->_width, texture->_height, pixel_format, type, data);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void st_gl_command_list::transition(st_texture* texture, e_st_texture_state new_state)
{
}

void st_gl_command_list::update_buffer(st_buffer* buffer_, void* data, const uint32_t offset, const uint32_t count)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);

	if (buffer->_usage & e_st_buffer_usage::storage)
	{
		uint8_t* head;
		_device->map(buffer_, 0, { 0, buffer->_element_size * buffer->_count }, (void**)&head);
		memcpy(head + offset, data, count * buffer->_element_size);
		_device->unmap(buffer_, 0, { 0, 0 });
	}
	else if (buffer->_usage & e_st_buffer_usage::uniform)
	{
		memcpy(buffer->_storage, data, buffer->_element_size * buffer->_count);
	}
	else if (buffer->_usage & e_st_buffer_usage::vertex)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer->_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, offset, count * buffer->_element_size, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else if (buffer->_usage & e_st_buffer_usage::index)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->_buffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * buffer->_element_size, data);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void st_gl_command_list::bind_resources(st_resource_table* table_)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	const st_gl_shader* shader = _bound_shader;

	for (uint32_t i = 0; i < table->_srvs.size(); ++i)
	{
		if (i >= shader->get_uniform_count())
			continue;

		const st_gl_texture_view* view = static_cast<const st_gl_texture_view*>(table->_srvs[i]);
		const st_gl_texture* texture = static_cast<const st_gl_texture*>(view->_desc._texture);
		const st_gl_sampler* sampler = static_cast<const st_gl_sampler*>(table->_samplers[i]);

		const st_gl_uniform& uniform = shader->get_uniform(i);
		uniform.set_srv(texture, sampler);
	}

	for (uint32_t i = 0; i < table->_constant_buffers.size(); ++i)
	{
		if (i >= shader->get_uniform_block_count())
			continue;

		const st_gl_buffer_view* view = static_cast<const st_gl_buffer_view*>(table->_constant_buffers[i]);
		const st_gl_buffer* cb = static_cast<const st_gl_buffer*>(view->_desc._buffer);
	
		const st_gl_uniform_block& block = shader->get_uniform_block(i);
		block.set(cb->_buffer, cb->_storage, cb->_element_size * cb->_count);
	}
	
	for (uint32_t i = 0; i < table->_buffers.size(); ++i)
	{
		if (i >= shader->get_shader_storage_block_count())
			continue;

		const st_gl_buffer_view* view = static_cast<const st_gl_buffer_view*>(table->_buffers[i]);
		const st_gl_buffer* buffer = static_cast<const st_gl_buffer*>(view->_desc._buffer);
	
		const st_gl_shader_storage_block& block = shader->get_shader_storage_block(i);
		block.set(buffer->_buffer);
	}

	for (uint32_t i = 0; i < table->_uavs.size(); ++i)
	{
		const st_gl_texture_view* view = static_cast<const st_gl_texture_view*>(table->_uavs[i]);
		const st_gl_texture* texture = static_cast<const st_gl_texture*>(view->_desc._texture);

		const st_gl_uniform& uniform = shader->get_uniform(i);
		uniform.set_uav(texture);
	}
}

void st_gl_command_list::bind_compute_resources(st_resource_table* table)
{
	bind_resources(table);
}

void st_gl_command_list::begin_render_pass(
	st_render_pass* pass_,
	st_framebuffer* framebuffer_,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
	st_gl_render_pass* pass = static_cast<st_gl_render_pass*>(pass_);

	set_viewport(pass->_viewport);

	st_gl_framebuffer* framebuffer = static_cast<st_gl_framebuffer*>(framebuffer_);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->_handle);

	// If this is the backbuffer, then there's nothing else to do.
	if (framebuffer->_handle > 0)
	{
		for (uint32_t att = 0; att < pass->_color_attachments.size(); ++att)
		{
			const st_attachment_desc& attachment = pass->_color_attachments[att];
			glDrawBuffer(GL_COLOR_ATTACHMENT0 + att);

			if (attachment._load_op == e_st_load_op::clear)
			{
				GLclampf r = clear_values[att]._color.x;
				GLclampf g = clear_values[att]._color.y;
				GLclampf b = clear_values[att]._color.z;
				GLclampf a = clear_values[att]._color.w;
				glClearColor(r, g, b, a);
				glClear(GL_COLOR_BUFFER_BIT);
			}
		}

		if (pass->_depth_attachment._load_op == e_st_load_op::clear)
		{
			glClearDepth(clear_values[clear_count - 1]._depth_stencil._depth);
			glClearStencil(clear_values[clear_count - 1]._depth_stencil._stencil);
			glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

		GLenum targets[] =
		{
			GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3,
			GL_COLOR_ATTACHMENT4,
			GL_COLOR_ATTACHMENT5,
			GL_COLOR_ATTACHMENT6,
			GL_COLOR_ATTACHMENT7,
		};

		glDrawBuffers(framebuffer->_target_count, targets);
	}
}

void st_gl_command_list::end_render_pass(st_render_pass* pass_, st_framebuffer* framebuffer)
{
	st_gl_render_pass* pass = static_cast<st_gl_render_pass*>(pass_);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum target[] = { GL_BACK_LEFT };
	glDrawBuffers(1, target);
}

void st_gl_command_list::set_depth_state(bool enable, GLenum func)
{
	if (enable)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(func);
	}
	else
		glDisable(GL_DEPTH_TEST);
}

void st_gl_command_list::set_stencil_state(const st_depth_stencil_desc& desc)
{
	if (desc._stencil_enable)
	{
		glEnable(GL_STENCIL_TEST);

		glStencilOpSeparate(
			GL_FRONT,
			convert_stencil_op(desc._front_stencil._stencil_fail_op),
			convert_stencil_op(desc._front_stencil._depth_fail_op),
			convert_stencil_op(desc._front_stencil._stencil_pass_op));
		glStencilOpSeparate(
			GL_BACK,
			convert_stencil_op(desc._back_stencil._stencil_fail_op),
			convert_stencil_op(desc._back_stencil._depth_fail_op),
			convert_stencil_op(desc._back_stencil._stencil_pass_op));

		glStencilFuncSeparate(
			GL_FRONT,
			convert_compare_func(desc._front_stencil._stencil_func),
			desc._stencil_ref,
			desc._stencil_read_mask);

		glStencilFuncSeparate(
			GL_BACK,
			convert_compare_func(desc._back_stencil._stencil_func),
			desc._stencil_ref,
			desc._stencil_read_mask);

		glStencilMask(desc._stencil_read_mask);
	}
	else
		glDisable(GL_STENCIL_TEST);
}

void st_gl_command_list::set_cull_state(bool enable, GLenum mode)
{
	if (enable)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(mode);
	}
	else
		glDisable(GL_CULL_FACE);
}

void st_gl_command_list::set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor)
{
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);

	glBlendFunc(src_factor, dst_factor);
}

void st_gl_command_list::set_depth_mask(bool enable)
{
	glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

#endif
