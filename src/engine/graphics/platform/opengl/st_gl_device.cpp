/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_device.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_command_allocator.h>
#include <graphics/platform/opengl/st_gl_command_list.h>
#include <graphics/platform/opengl/st_gl_command_queue.h>
#include <graphics/platform/opengl/st_gl_conversion.h>
#include <graphics/platform/opengl/st_gl_shader.h>

#include <graphics/geometry/st_vertex_attribute.h>

#include <cassert>

st_gl_device::st_gl_device(HDC context)
	: _device_context(context)
{
}

st_gl_device::~st_gl_device()
{
}

std::unique_ptr<st_command_queue> st_gl_device::create_command_queue(const st_command_queue_desc& desc)
{
	std::unique_ptr<st_gl_command_queue> queue = std::make_unique<st_gl_command_queue>(_device_context);
	return std::move(queue);
}

std::unique_ptr<st_command_allocator> st_gl_device::create_command_allocator(const st_command_allocator_desc& desc)
{
	std::unique_ptr<st_gl_command_allocator> allocator = std::make_unique<st_gl_command_allocator>();
	return std::move(allocator);
}

std::unique_ptr<st_command_list> st_gl_device::create_command_list(const st_command_list_desc& desc)
{
	std::unique_ptr<st_gl_command_list> command_list = std::make_unique<st_gl_command_list>(this);
	return std::move(command_list);
}

std::unique_ptr<st_fence> st_gl_device::create_fence(const st_fence_desc& desc)
{
	std::unique_ptr<st_gl_fence> fence = std::make_unique<st_gl_fence>();
	return std::move(fence);
}

void st_gl_device::wait(st_fence* fence_, uint64_t value)
{
	st_gl_fence* fence = static_cast<st_gl_fence*>(fence_);

	glClientWaitSync(fence->_sync, GL_SYNC_FLUSH_COMMANDS_BIT, UINT64_MAX);
}

std::unique_ptr<st_swap_chain> st_gl_device::create_swap_chain(const st_swap_chain_desc& desc)
{
	std::unique_ptr<st_gl_swap_chain> swap_chain = std::make_unique<st_gl_swap_chain>();

	swap_chain->_backbuffers.reserve(desc._buffer_count);
	swap_chain->_backbuffer_views.reserve(desc._buffer_count);

	for (uint32_t i = 0; i < desc._buffer_count; ++i)
	{
		std::unique_ptr<st_gl_texture> buffer = std::make_unique<st_gl_texture>();
		buffer->_width = desc._width;
		buffer->_height = desc._height;
		buffer->_format = desc._format;
		buffer->_handle = 0;

		st_texture_view_desc view_desc;
		view_desc._texture = buffer.get();
		view_desc._usage = e_st_view_usage::render_target;
		view_desc._format = desc._format;

		std::unique_ptr<st_texture_view> view = create_texture_view(view_desc);

		swap_chain->_backbuffers.push_back(std::move(buffer));
		swap_chain->_backbuffer_views.push_back(std::move(view));
	}

	return std::move(swap_chain);
}

void st_gl_device::reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain_)
{
	st_gl_swap_chain* swap_chain = static_cast<st_gl_swap_chain*>(swap_chain_);

	swap_chain->_backbuffers.clear();
	swap_chain->_backbuffer_views.clear();

	for (uint32_t i = 0; i < desc._buffer_count; ++i)
	{
		std::unique_ptr<st_gl_texture> buffer = std::make_unique<st_gl_texture>();
		buffer->_width = desc._width;
		buffer->_height = desc._height;
		buffer->_format = desc._format;
		buffer->_handle = 0;

		st_texture_view_desc view_desc;
		view_desc._texture = buffer.get();
		view_desc._usage = e_st_view_usage::render_target;
		view_desc._format = desc._format;

		std::unique_ptr<st_texture_view> view = create_texture_view(view_desc);

		swap_chain->_backbuffers.push_back(std::move(buffer));
		swap_chain->_backbuffer_views.push_back(std::move(view));
	}
}

st_texture* st_gl_device::get_backbuffer(st_swap_chain* swap_chain_, uint32_t index)
{
	st_gl_swap_chain* swap_chain = static_cast<st_gl_swap_chain*>(swap_chain_);
	return swap_chain->_backbuffers[index].get();
}

st_texture_view* st_gl_device::get_backbuffer_view(st_swap_chain* swap_chain_, uint32_t index)
{
	st_gl_swap_chain* swap_chain = static_cast<st_gl_swap_chain*>(swap_chain_);
	return swap_chain->_backbuffer_views[index].get();
}

uint32_t st_gl_device::get_backbuffer_index(st_swap_chain* swap_chain)
{
	// TODO: What would it mean to triple-buffer?
	return 0;
}

e_st_swap_chain_status st_gl_device::acquire_backbuffer(st_swap_chain* swap_chain)
{
	return e_st_swap_chain_status::current;
}

std::unique_ptr<st_texture> st_gl_device::create_texture(const st_texture_desc& desc)
{
	std::unique_ptr<st_gl_texture> texture = std::make_unique<st_gl_texture>();
	texture->_format = desc._format;
	texture->_width = desc._width;
	texture->_height = desc._height;
	texture->_levels = desc._levels;

	glGenTextures(1, &texture->_handle);

	glBindTexture(GL_TEXTURE_2D, texture->_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexStorage2D(GL_TEXTURE_2D, desc._levels, convert_format(desc._format), desc._width, desc._height);

	glBindTexture(GL_TEXTURE_2D, 0);

	return std::move(texture);
}

void st_gl_device::set_texture_name(st_texture* texture_, std::string name)
{
	st_gl_texture* texture = static_cast<st_gl_texture*>(texture_);
	glObjectLabel(GL_TEXTURE, texture->_handle, name.length(), name.c_str());
}

std::unique_ptr<st_texture_view> st_gl_device::create_texture_view(const st_texture_view_desc& desc)
{
	std::unique_ptr<st_gl_texture_view> view = std::make_unique<st_gl_texture_view>();
	memcpy(const_cast<st_texture_view_desc*>(&view->_desc), &desc, sizeof(st_texture_view_desc));

	return std::move(view);
}

std::unique_ptr<st_sampler> st_gl_device::create_sampler(const st_sampler_desc& desc)
{
	std::unique_ptr<st_gl_sampler> sampler = std::make_unique<st_gl_sampler>();

	glGenSamplers(1, &sampler->_handle);

	GLenum min_filter = (desc._mip_filter == st_filter_nearest) ?
		((desc._min_filter == st_filter_nearest) ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_NEAREST) :
		((desc._min_filter == st_filter_nearest) ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
	GLenum mag_filter = (desc._mag_filter == st_filter_nearest) ? GL_NEAREST : GL_LINEAR;
	glSamplerParameteri(sampler->_handle, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(sampler->_handle, GL_TEXTURE_MAG_FILTER, mag_filter);

	glSamplerParameteri(sampler->_handle, GL_TEXTURE_WRAP_S, convert_address_mode(desc._address_u));
	glSamplerParameteri(sampler->_handle, GL_TEXTURE_WRAP_T, convert_address_mode(desc._address_v));
	glSamplerParameteri(sampler->_handle, GL_TEXTURE_WRAP_R, convert_address_mode(desc._address_w));
	glSamplerParameterf(sampler->_handle, GL_TEXTURE_MIN_LOD, desc._min_mip);
	glSamplerParameterf(sampler->_handle, GL_TEXTURE_MAX_LOD, desc._max_mip);
	glSamplerParameterf(sampler->_handle, GL_TEXTURE_LOD_BIAS, desc._mip_bias);
	glSamplerParameteri(
		sampler->_handle,
		GL_TEXTURE_COMPARE_MODE,
		(desc._compare_func != st_compare_func_never) ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE);
	glSamplerParameteri(sampler->_handle, GL_TEXTURE_COMPARE_FUNC, convert_compare_func(desc._compare_func));

	// TODO: Border color unimplemented.

	return std::move(sampler);
}

std::unique_ptr<st_buffer> st_gl_device::create_buffer(const st_buffer_desc& desc)
{
	std::unique_ptr<st_gl_buffer> buffer = std::make_unique<st_gl_buffer>();
	buffer->_usage = desc._usage;
	buffer->_element_size = desc._element_size;
	buffer->_count = desc._count;
	buffer->_storage = nullptr;

	if (desc._usage & e_st_buffer_usage::uniform)
	{
		// For uniforms, use local storage to cache contents.
		buffer->_storage = reinterpret_cast<uint8_t*>(malloc(buffer->_element_size * buffer->_count));

		glGenBuffers(1, &buffer->_buffer);
	}
	else
	{
		GLenum target = GL_SHADER_STORAGE_BUFFER;

		if (desc._usage & e_st_buffer_usage::vertex)
			target = GL_ARRAY_BUFFER;
		else if (desc._usage & e_st_buffer_usage::index)
			target = GL_ELEMENT_ARRAY_BUFFER;

		glGenBuffers(1, &buffer->_buffer);
		glBindBuffer(target, buffer->_buffer);
		glBufferData(target, desc._count * desc._element_size, NULL, GL_STATIC_DRAW);
		glBindBuffer(target, 0);
	}

	return std::move(buffer);
}

std::unique_ptr<st_buffer_view> st_gl_device::create_buffer_view(const st_buffer_view_desc& desc)
{
	std::unique_ptr<st_gl_buffer_view> view = std::make_unique<st_gl_buffer_view>();
	memcpy(const_cast<st_buffer_view_desc*>(&view->_desc), &desc, sizeof(st_buffer_view_desc));

	return std::move(view);
}

void st_gl_device::map(st_buffer* buffer_, uint32_t subresource, const st_range& range, void** outData)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer->_buffer);

	GLsizeiptr size = (range.end - range.begin);
	if (range.end == 0)
		size = buffer->_element_size * buffer->_count;
	*outData = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, range.begin, size, GL_MAP_WRITE_BIT);
}

void st_gl_device::unmap(st_buffer* buffer, uint32_t subresource, const st_range& range)
{
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void st_gl_device::set_buffer_name(st_buffer* buffer_, std::string name)
{
	st_gl_buffer* buffer = static_cast<st_gl_buffer*>(buffer_);
	glObjectLabel(GL_BUFFER, buffer->_buffer, name.length(), name.c_str());
}

std::unique_ptr<st_resource_table> st_gl_device::create_resource_table()
{
	std::unique_ptr<st_gl_resource_table> table = std::make_unique<st_gl_resource_table>();
	return std::move(table);
}

std::unique_ptr<st_resource_table> st_gl_device::create_resource_table_compute()
{
	std::unique_ptr<st_gl_resource_table> table = std::make_unique<st_gl_resource_table>();
	return std::move(table);
}

void st_gl_device::set_constant_buffers(st_resource_table* table_, uint32_t count, const st_buffer_view** cbs)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		table->_constant_buffers.push_back(cbs[i]);
	}
}

void st_gl_device::set_textures(
	st_resource_table* table_,
	uint32_t count,
	const st_texture_view** textures,
	const st_sampler** samplers)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		if (textures)
			table->_srvs.push_back(textures[i]);
		else
			table->_srvs.push_back(0);

		if (samplers)
		{
			const st_gl_sampler* s = static_cast<const st_gl_sampler*>(samplers[i]);
			table->_samplers.push_back(s);
		}
		else
			table->_samplers.push_back(0);
	}
}

void st_gl_device::set_buffers(st_resource_table* table_, uint32_t count, const st_buffer_view** buffers)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		table->_buffers.push_back(buffers[i]);
	}
}

void st_gl_device::set_uavs(st_resource_table* table_, uint32_t count, const st_texture_view** textures)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t i = 0; i < count; ++i)
	{
		table->_uavs.push_back(textures[i]);
	}
}

void st_gl_device::update_textures(st_resource_table* table_, uint32_t count, const st_texture_view** views)
{
	st_gl_resource_table* table = static_cast<st_gl_resource_table*>(table_);

	for (uint32_t itr = 0; itr < count; ++itr)
	{
		table->_srvs[itr] = views[itr];
	}
}

std::unique_ptr<st_shader> st_gl_device::create_shader(const char* filename, e_st_shader_type_flags type)
{
	std::unique_ptr<st_gl_shader> shader = std::make_unique<st_gl_shader>(filename, type);
	return std::move(shader);
}

std::unique_ptr<st_pipeline> st_gl_device::create_graphics_pipeline(const st_graphics_state_desc& desc)
{
	std::unique_ptr<st_gl_pipeline> pipeline = std::make_unique<st_gl_pipeline>();
	pipeline->_graphics_desc = desc;
	return std::move(pipeline);
}

std::unique_ptr<st_pipeline> st_gl_device::create_compute_pipeline(const st_compute_state_desc& desc)
{
	std::unique_ptr<st_gl_pipeline> pipeline = std::make_unique<st_gl_pipeline>();
	pipeline->_compute_desc = desc;
	return std::move(pipeline);
}

std::unique_ptr<st_vertex_format> st_gl_device::create_vertex_format(
	const st_vertex_attribute* attributes,
	uint32_t attribute_count)
{
	std::unique_ptr<st_gl_vertex_format> format = std::make_unique<st_gl_vertex_format>();

	size_t vertex_size = calculate_vertex_size(attributes, attribute_count);
	format->_vertex_size = (uint32_t)vertex_size;

	format->_attributes.reserve(attribute_count);
	for (uint32_t itr = 0; itr < attribute_count; ++itr)
		format->_attributes.push_back(attributes[itr]);
	return std::move(format);
}

std::unique_ptr<st_render_pass> st_gl_device::create_render_pass(const st_render_pass_desc& desc)
{
	std::unique_ptr<st_gl_render_pass> render_pass = std::make_unique<st_gl_render_pass>();
	render_pass->_viewport = desc._viewport;

	for (uint32_t i = 0; i < desc._attachment_count; ++i)
		render_pass->_color_attachments.push_back(desc._attachments[i]);
	render_pass->_depth_attachment = desc._depth_attachment;

	return std::move(render_pass);
}

std::unique_ptr<st_framebuffer> st_gl_device::create_framebuffer(const st_framebuffer_desc& desc)
{
	std::unique_ptr<st_gl_framebuffer> framebuffer = std::make_unique<st_gl_framebuffer>();
	framebuffer->_target_count = desc._target_count;

	bool is_backbuffer = false;
	if (desc._target_count > 0)
	{
		st_gl_texture* texture = static_cast<st_gl_texture*>(desc._targets[0]._texture);
		if (texture->_handle == 0)
		{
			is_backbuffer = true;
		}
	}

	if (!is_backbuffer)
	{
		glGenFramebuffers(1, &framebuffer->_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->_handle);
	}

	uint32_t color_attachment = GL_COLOR_ATTACHMENT0;
	for (uint32_t color_target = 0; color_target < desc._target_count; ++color_target)
	{
		st_gl_texture* texture = static_cast<st_gl_texture*>(desc._targets[color_target]._texture);

		if (texture->_handle > 0)
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				color_attachment,
				GL_TEXTURE_2D,
				texture->_handle,
				0);

		color_attachment++;
	}

	if (desc._depth_target._texture)
	{
		st_gl_texture* ds = static_cast<st_gl_texture*>(desc._depth_target._texture);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_STENCIL_ATTACHMENT,
			GL_TEXTURE_2D,
			ds->_handle,
			0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		const GLubyte* error_string = gluErrorString(status);
		assert(false);
		GLenum test = GL_INVALID_OPERATION;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::move(framebuffer);
}

void st_gl_device::get_desc(const st_texture* texture_, st_texture_desc* out_desc)
{
	assert(out_desc);
	const st_gl_texture* texture = static_cast<const st_gl_texture*>(texture_);
	out_desc->_format = texture->_format;
	out_desc->_width = texture->_width;
	out_desc->_height = texture->_height;
	out_desc->_levels = texture->_levels;
	// TODO: Depth and others.
}

#endif
