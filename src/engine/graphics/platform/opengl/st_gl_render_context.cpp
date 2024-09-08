/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_render_context.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/platform/opengl/st_gl_conversion.h>
#include <graphics/platform/opengl/st_gl_framebuffer.h>
#include <graphics/platform/opengl/st_gl_shader.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_texture.h>

#include <system/st_window.h>

#include <cassert>
#include <cstdio>

st_gl_render_context::st_gl_render_context(const st_window* window)
{
	_device_context = GetDC(window->get_window_handle());

	// Choose a pixel format.
	PIXELFORMATDESCRIPTOR format_desc =
	{ 
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
 
	// Get the device context's best available pixel format match.
	int format_index = ChoosePixelFormat(_device_context, &format_desc); 
 
	// Make that match the device context's current pixel format.
	SetPixelFormat(_device_context, format_index, &format_desc);

	_gl_context = wglCreateContext(_device_context);

	wglMakeCurrent(_device_context, _gl_context);

	// Init GLEW after creating the OpenGL context so it can locate the functions.
	GLint glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK)
	{
		printf("Failed to initialize GLEW. %s. Exiting.\n", glewGetErrorString(glew_init_result));
		exit(1);
	}
	
	const GLint attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 4,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

    _gl_context = wglCreateContextAttribsARB(_device_context, 0, attribs);

	wglMakeCurrent(_device_context, _gl_context);

	GLint major;
	GLint minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("Using OpenGL %d.%d. Version: %s\n", major, minor, (char*)glGetString(GL_VERSION));

#if defined(_DEBUG)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback((GLDEBUGPROC)gl_message_callback, 0);
#endif

	// Create the faux backbuffer target.
	_present_target = std::make_unique<st_render_texture>(
		this,
		window->get_width(),
		window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::copy_source,
		st_texture_state_copy_source,
		st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f },
		"Backbuffer");

	st_render_texture* targets[] =
	{
		_present_target.get(),
	};

	_present_framebuffer = std::make_unique<st_gl_framebuffer>(
		1,
		targets,
		nullptr);

	_this = this;
}

st_gl_render_context::~st_gl_render_context()
{
	_present_framebuffer = nullptr;
	_present_target = nullptr;

	// Destroy the GL context.
	wglDeleteContext(_gl_context);
}

void st_gl_render_context::acquire()
{
	// Acquire for current thread.
	wglMakeCurrent(_device_context, _gl_context);
}

void st_gl_render_context::release()
{
	// Release from current thread.
	wglMakeCurrent(_device_context, 0);
}

void st_gl_render_context::set_pipeline(const st_pipeline* state_)
{
	const st_gl_pipeline* state = static_cast<const st_gl_pipeline*>(state_);

	const st_pipeline_state_desc& state_desc = state->_state_desc;
	_bound_shader = static_cast<const st_gl_shader*>(state_desc._shader);
	glUseProgram(_bound_shader->_handle);

	// TODO: Convert these.
	set_depth_state(
		state_desc._depth_stencil_desc._depth_enable,
		convert_compare_func(state_desc._depth_stencil_desc._depth_compare));

	set_depth_mask(
		state_desc._depth_stencil_desc._depth_mask != st_depth_write_mask_zero);

	set_blend_state(
		state_desc._blend_desc._target_blend[0]._blend,
		convert_blend(state_desc._blend_desc._target_blend[0]._src_blend),
		convert_blend(state_desc._blend_desc._target_blend[0]._dst_blend));

	set_cull_state(
		state_desc._rasterizer_desc._cull_mode != GL_NONE,
		convert_cull_mode(state_desc._rasterizer_desc._cull_mode));
}

void st_gl_render_context::set_viewport(const st_viewport& viewport)
{
	glViewport(viewport._x, viewport._y, viewport._width, viewport._height);
}

void st_gl_render_context::set_scissor(int left, int top, int right, int bottom)
{
	glScissor(left, top, right - left, bottom - top);
}

void st_gl_render_context::set_clear_color(float r, float g, float b, float a)
{
	_clear_color[0] = r;
	_clear_color[1] = g;
	_clear_color[2] = b;
	_clear_color[3] = a;

	glClearColor(r, g, b, a);
}

void st_gl_render_context::set_shader_resource_table(uint32_t offset)
{

}

void st_gl_render_context::set_sampler_table(uint32_t offset)
{

}

void st_gl_render_context::set_constant_buffer_table(uint32_t offset)
{

}

void st_gl_render_context::set_buffer_table(uint32_t offset)
{

}

void st_gl_render_context::set_render_targets(
	uint32_t count,
	const st_texture_view** targets,
	const st_texture_view* depth_stencil)
{

}

void st_gl_render_context::clear(unsigned int clear_flags)
{
	GLbitfield flags = 0;
	flags |= (clear_flags & st_clear_flag_color) ? GL_COLOR_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_depth) ? GL_DEPTH_BUFFER_BIT : 0;
	flags |= (clear_flags & st_clear_flag_stencil) ? GL_STENCIL_BUFFER_BIT : 0;

	glClear(flags);
}

void st_gl_render_context::draw(const st_static_drawcall& drawcall)
{
	st_gl_geometry* geometry = static_cast<st_gl_geometry*>(drawcall._geometry);

	glBindVertexArray(geometry->_vao);
	glDrawElements(convert_topology(drawcall._draw_mode), geometry->_index_count, GL_UNSIGNED_SHORT, 0);
}

void st_gl_render_context::draw(const st_dynamic_drawcall& drawcall)
{
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint pos;
	glGenBuffers(1, &pos);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec3f) * drawcall._positions.size(), &drawcall._positions[0], GL_STREAM_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	GLuint texcoord;
	if (!drawcall._texcoords.empty())
	{
		glGenBuffers(1, &texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec2f) * drawcall._texcoords.size(), &drawcall._texcoords[0], GL_STREAM_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);
	}

	GLuint indices;
	glGenBuffers(1, &indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * drawcall._indices.size(), &drawcall._indices[0], GL_STREAM_DRAW);

	glDrawElements(convert_topology(drawcall._draw_mode), (GLsizei)drawcall._indices.size(), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteBuffers(1, &indices);
	if (!drawcall._texcoords.empty())
	{
		glDeleteBuffers(1, &texcoord);
	}
	glDeleteBuffers(1, &pos);
	glDeleteVertexArrays(1, &vao);
	glBindVertexArray(0);
}

void st_gl_render_context::swap()
{
	// Copy the present target to the backbuffer.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _present_framebuffer->get_handle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(
		0,
		0,
		_present_target->get_width(),
		_present_target->get_height(),
		0,
		0,
		_present_target->get_width(),
		_present_target->get_height(),
		GL_COLOR_BUFFER_BIT,
		GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SwapBuffers(_device_context);
}

void st_gl_render_context::begin_marker(const std::string& marker)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, marker.length(), marker.c_str());
}

void st_gl_render_context::end_marker()
{
	glPopDebugGroup();
}

std::unique_ptr<st_texture> st_gl_render_context::create_texture(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	const st_vec4f& clear,
	void* data)
{
	std::unique_ptr<st_gl_texture> texture = std::make_unique<st_gl_texture>();

	glGenTextures(1, &texture->_handle);

	glBindTexture(GL_TEXTURE_2D, texture->_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexStorage2D(GL_TEXTURE_2D, levels, convert_format(format), width, height);

	if (data)
	{
		if (is_compressed(format))
		{
			uint8_t* bits = reinterpret_cast<uint8_t*>(data);

			for (uint32_t mip = 0; mip < levels; ++mip)
			{
				uint32_t level_width = width >> mip;
				uint32_t level_height = height >> mip;

				size_t row_bytes;
				size_t num_rows;
				get_surface_info(level_width, level_height, format, nullptr, &row_bytes, &num_rows);

				glCompressedTexSubImage2D(
					GL_TEXTURE_2D,
					mip,
					0,
					0,
					level_width,
					level_height,
					convert_format(format),
					row_bytes * num_rows,
					bits);

				bits += row_bytes * num_rows;
			}
		}
		else
		{
			GLenum pixel_format;
			GLenum type;
			get_pixel_format_and_type(format, pixel_format, type);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, type, data);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return std::move(texture);
}

void st_gl_render_context::set_texture_meta(st_texture* texture_, const char* name)
{
	st_gl_texture* texture = static_cast<st_gl_texture*>(texture_);
	texture->_name = name;
}

void st_gl_render_context::set_texture_name(st_texture* texture_, std::string name)
{
	st_gl_texture* texture = static_cast<st_gl_texture*>(texture_);
	glObjectLabel(GL_TEXTURE, texture->_handle, name.length(), name.c_str());
}

std::unique_ptr<st_texture_view> st_gl_render_context::create_texture_view(st_texture* texture)
{
	std::unique_ptr<st_gl_texture_view> view = std::make_unique<st_gl_texture_view>();
	view->_texture = static_cast<const st_gl_texture*>(texture);

	return std::move(view);
}

std::unique_ptr<st_buffer> st_gl_render_context::create_buffer(
	const uint32_t count,
	const size_t element_size,
	const e_st_buffer_usage_flags usage)
{
	std::unique_ptr<st_gl_buffer> buffer = std::make_unique<st_gl_buffer>();
	buffer->_usage = usage;
	buffer->_element_size = element_size;
	buffer->_count = count;

	glGenBuffers(1, &buffer->_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count * element_size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return std::move(buffer);
}

std::unique_ptr<st_buffer_view> st_gl_render_context::create_buffer_view(st_buffer* buffer)
{
	std::unique_ptr<st_gl_buffer_view> view = std::make_unique<st_gl_buffer_view>();
	view->_buffer = static_cast<st_gl_buffer*>(buffer);

	return std::move(view);
}

void st_gl_render_context::map(st_buffer* buffer_, uint32_t subresource, const st_range& range, void** outData)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->_buffer);
	*outData = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, range.begin, (range.end - range.begin), GL_MAP_WRITE_BIT);
}

void st_gl_render_context::unmap(st_buffer* buffer_, uint32_t subresource, const st_range& range)
{
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void st_gl_render_context::update_buffer(st_buffer* buffer_, void* data, const uint32_t count)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);

	if (buffer->_usage & e_st_buffer_usage::storage)
	{
		uint8_t* head;
		map(buffer_, 0, { 0, buffer->_element_size * buffer->_count }, (void**)&head);
		memcpy(head, data, count * buffer->_element_size);
		unmap(buffer_, 0, { 0, 0 });
	}
	else if (buffer->_usage & e_st_buffer_usage::uniform)
	{
		const st_gl_shader* shader = get_bound_shader();

		// Update by sending values from the data buffer piece by piece depending on type.
		size_t offset = 0;
		for (auto& constant : buffer->_constants)
		{
			st_gl_uniform uniform = shader->get_uniform(constant._name.c_str());

			switch (constant._type)
			{
			case st_shader_constant_type_float:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				float* val = reinterpret_cast<float*>(data_offset);
				uniform.set(*val);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
			case st_shader_constant_type_vec2:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec2f* vec2 = reinterpret_cast<st_vec2f*>(data_offset);
				uniform.set(*vec2);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
			case st_shader_constant_type_vec3:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec3f* vec3 = reinterpret_cast<st_vec3f*>(data_offset);
				uniform.set(*vec3);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
			case st_shader_constant_type_vec4:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_vec4f* vec4 = reinterpret_cast<st_vec4f*>(data_offset);
				uniform.set(*vec4);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
			case st_shader_constant_type_mat4:
			{
				char* data_offset = reinterpret_cast<char*>(data) + offset;
				st_mat4f* mat4 = reinterpret_cast<st_mat4f*>(data_offset);
				uniform.set(*mat4);

				offset += st_graphics_get_shader_constant_size(constant._type);
			}
			break;
			case st_shader_constant_type_block:
			{
				st_gl_uniform_block block = shader->get_uniform_block(constant._name.c_str());
				block.set(buffer->_buffer, data, buffer->_element_size * buffer->_count);

				offset += buffer->_element_size * buffer->_count;
			}
			break;
			default:
				assert(false);
				break;
			}
		}
	}
}

void st_gl_render_context::set_buffer_meta(st_buffer* buffer_, std::string name)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);
	buffer->_name = name;
}

void st_gl_render_context::add_constant(
	st_buffer* buffer_,
	const std::string& name,
	const e_st_shader_constant_type constant_type)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);

	assert(buffer->_usage & e_st_buffer_usage::uniform);

	st_gl_constant constant;
	constant._name = name;
	constant._type = constant_type;

	buffer->_constants.push_back(constant);
}

std::unique_ptr<st_resource_table> st_gl_render_context::create_resource_table()
{
	std::unique_ptr<st_gl_resource_table> table = std::make_unique<st_gl_resource_table>();
	return std::move(table);
}

void st_gl_render_context::set_constant_buffers(st_resource_table* table_, uint32_t count, st_buffer** cbs)
{
}

void st_gl_render_context::set_textures(st_resource_table* table_, uint32_t count, st_texture** textures)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		table->_srvs.push_back(textures[i]);

		st_gl_texture* texture = static_cast<st_gl_texture*>(textures[i]);

		// Create a sampler for the texture.
		GLuint sampler;
		glGenSamplers(1, &sampler);
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, texture->_levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

		table->_samplers.push_back(sampler);
	}
}

void st_gl_render_context::set_buffers(st_resource_table* table, uint32_t count, st_buffer** buffers)
{
}

void st_gl_render_context::bind_resource_table(st_resource_table* table_)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < table->_srvs.size(); ++i)
	{
		st_gl_texture* texture = static_cast<st_gl_texture*>(table->_srvs[i]);

		const st_gl_shader* shader = get_bound_shader();

		st_gl_uniform uniform = shader->get_uniform(texture->_name.c_str());
		uniform.set(*texture, uniform.get_location());

		glBindSampler(uniform.get_location(), table->_samplers[i]);
	}
}

std::unique_ptr<st_shader> st_gl_render_context::create_shader(const char* filename, uint8_t type)
{
	std::unique_ptr<st_gl_shader> shader = std::make_unique<st_gl_shader>(filename, type);
	return std::move(shader);
}

std::unique_ptr<st_pipeline> st_gl_render_context::create_pipeline(const struct st_pipeline_state_desc& desc)
{
	std::unique_ptr<st_gl_pipeline> pipeline = std::make_unique<st_gl_pipeline>();
	pipeline->_state_desc = desc;
	return std::move(pipeline);
}

std::unique_ptr<st_vertex_format> st_gl_render_context::create_vertex_format(
	const struct st_vertex_attribute* attributes,
	uint32_t attribute_count)
{
	std::unique_ptr<st_gl_vertex_format> format = std::make_unique<st_gl_vertex_format>();

	// TODO: Group this into common code.
	size_t vertex_size = 0;

	for (uint32_t itr = 0; itr < attribute_count; ++itr)
	{
		const st_vertex_attribute* attr = &attributes[itr];

		// TODO: Switch on the attribute data type.
		size_t data_size = sizeof(float);

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
		case st_vertex_attribute_normal:
		case st_vertex_attribute_binormal:
		case st_vertex_attribute_tangent:
			vertex_size += data_size * 3;
			break;
		case st_vertex_attribute_color:
		case st_vertex_attribute_joints:
		case st_vertex_attribute_weights:
			vertex_size += data_size * 4;
			break;
		case st_vertex_attribute_uv:
			vertex_size += data_size * 2;
			break;
		default:
			assert(false);
			break;
		}
	}

	format->_vertex_size = (uint32_t)vertex_size;

	format->_attributes.reserve(attribute_count);
	memcpy(format->_attributes.data(), attributes, sizeof(st_vertex_attribute) * attribute_count);
	return std::move(format);
}

std::unique_ptr<st_geometry> st_gl_render_context::create_geometry(
	const st_vertex_format* format_,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
{
	std::unique_ptr<st_gl_geometry> geometry = std::make_unique<st_gl_geometry>();

	glGenVertexArrays(1, &geometry->_vao);
	glBindVertexArray(geometry->_vao);

	glGenBuffers(2, geometry->_vbos);

	glBindBuffer(GL_ARRAY_BUFFER, geometry->_vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * vertex_size, vertex_data, GL_STATIC_DRAW);

	// TODO: Instead of accumulated offset, we need offset into data of st_vertex.
	const st_gl_vertex_format* format = static_cast<const st_gl_vertex_format*>(format_);

	size_t offset = 0;
	for (uint32_t itr = 0; itr < format->_attributes.size(); ++itr)
	{
		const st_vertex_attribute* attr = &format->_attributes[itr];
		GLint size = 0;
		size_t data_size = sizeof(float);
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
		case st_vertex_attribute_normal:
		case st_vertex_attribute_binormal:
		case st_vertex_attribute_tangent:
			size = 3;
			break;
		case st_vertex_attribute_joints:
			type = GL_UNSIGNED_INT;
			data_size = sizeof(uint32_t);
		case st_vertex_attribute_color:
		case st_vertex_attribute_weights:
			size = 4;
			break;
		case st_vertex_attribute_uv:
			size = 2;
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
				size,
				type,
				vertex_size,
				(GLvoid*)offset);
		}
		else
		{
			glVertexAttribPointer(
				attr->_unit,
				size,
				type,
				normalized,
				vertex_size,
				(GLvoid*)offset);
		}

		glEnableVertexAttribArray(attr->_unit);

		offset += size * data_size;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->_vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(uint32_t)), index_data, GL_STATIC_DRAW);

	geometry->_index_count = index_count;

	glBindVertexArray(0);

	return std::move(geometry);
}

std::unique_ptr<st_render_pass> st_gl_render_context::create_render_pass(
	uint32_t count,
	class st_render_texture** targets,
	class st_render_texture* depth_stencil)
{
	std::unique_ptr<st_gl_render_pass> render_pass = std::make_unique<st_gl_render_pass>();

	// Naively, create the viewport from the first target.
	if (count > 0)
	{
		render_pass->_viewport =
		{
			0,
			0,
			float(targets[0]->get_width()),
			float(targets[0]->get_height()),
			0.0f,
			1.0f,
		};
	}
	render_pass->_framebuffer = std::make_unique<st_gl_framebuffer>(
		count,
		targets,
		depth_stencil);

	return std::move(render_pass);
}

void st_gl_render_context::begin_render_pass(
	st_render_pass* pass_,
	st_vec4f* clear_values,
	const uint8_t clear_count)
{
	st_gl_render_pass* pass = static_cast<st_gl_render_pass*>(pass_);

	set_viewport(pass->_viewport);
	pass->_framebuffer->bind(this);
}

void st_gl_render_context::end_render_pass(st_render_pass* pass_)
{
	st_gl_render_pass* pass = static_cast<st_gl_render_pass*>(pass_);

	pass->_framebuffer->unbind(this);
}

void st_gl_render_context::set_depth_state(bool enable, GLenum func)
{
	if (enable) glEnable(GL_DEPTH_TEST);
	else glDisable(GL_DEPTH_TEST);

	glDepthFunc(func);
}

void st_gl_render_context::set_cull_state(bool enable, GLenum mode)
{
	if (enable) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glCullFace(mode);
}

void st_gl_render_context::set_blend_state(bool enable, GLenum src_factor, GLenum dst_factor)
{
	if (enable) glEnable(GL_BLEND);
	else glDisable(GL_BLEND);

	glBlendFunc(src_factor, dst_factor);
}

void st_gl_render_context::set_depth_mask(bool enable)
{
	glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

#endif
