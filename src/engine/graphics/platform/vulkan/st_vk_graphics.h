#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DISABLE_ENHANCED_MODE
#define VULKAN_HPP_DISABLE_IMPLICIT_RESULT_VALUE_CAST
#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.hpp>

#include <graphics/platform/vulkan/st_vk_framebuffer.h>

// Ugh. Windows.
#undef min
#undef max

#define GLUEME(x, y) x##y
#define GLUE(x, y) GLUEME(x, y)
#define VK_VALIDATE(call) \
	vk::Result GLUE(vkr, __LINE__) = call; \
	vk_validate(GLUE(vkr, __LINE__))
#define VK_GET_HANDLE(object, ctype) \
	uint64_t(static_cast< ctype >(object))

bool vk_validate(vk::Result result);

struct st_vk_resource
{
	~st_vk_resource() { _device = nullptr; }

	vk::Device* _device = nullptr;
};

struct st_vk_buffer : public st_buffer, public st_vk_resource
{
	~st_vk_buffer()
	{
		_device->freeMemory(_memory, nullptr);
		_device->destroyBuffer(_buffer, nullptr);
	}

	uint32_t _count;
	e_st_buffer_usage_flags _usage;
	size_t _element_size;

	vk::Buffer _buffer;
	vk::DeviceMemory _memory;
};

struct st_vk_buffer_view : public st_buffer_view, public st_vk_resource
{
	~st_vk_buffer_view() { _device->destroyBufferView(_view, nullptr); }

	vk::BufferView _view;
};

struct st_vk_pipeline : public st_pipeline, public st_vk_resource
{
	~st_vk_pipeline() { _device->destroyPipeline(_pipeline, nullptr); }

	vk::Pipeline _pipeline;
};

struct st_vk_render_pass : public st_render_pass, public st_vk_resource
{
	~st_vk_render_pass() { _device->destroyRenderPass(_render_pass, nullptr); }

	vk::RenderPass _render_pass;
	std::unique_ptr<class st_vk_framebuffer> _framebuffer;
	vk::Viewport _viewport;
};

struct st_vk_resource_table : public st_resource_table, public st_vk_resource
{
	~st_vk_resource_table()
	{
		for (auto& sampler : _sampler_resources)
		{
			_device->destroySampler(sampler, nullptr);
		}
		_sampler_resources.clear();

		vk::DescriptorSet sets[] =
		{
			_textures,
			_buffers,
			_constants,
			_samplers,
		};

		VK_VALIDATE(_device->freeDescriptorSets(*_pool, 4, sets));
	}

	vk::DescriptorSet _textures;
	vk::DescriptorSet _constants;
	vk::DescriptorSet _buffers;
	vk::DescriptorSet _samplers;

	uint32_t _texture_count = 0;
	uint32_t _constant_count = 0;
	uint32_t _buffer_count = 0;
	uint32_t _sampler_count = 0;

	std::vector<vk::Sampler> _sampler_resources;

	vk::DescriptorPool* _pool;
};

struct st_vk_shader : public st_shader, public st_vk_resource
{
	~st_vk_shader()
	{
		_device->destroyShaderModule(_vs, nullptr);
		_device->destroyShaderModule(_ps, nullptr);
		_device->destroyShaderModule(_ds, nullptr);
		_device->destroyShaderModule(_hs, nullptr);
		_device->destroyShaderModule(_gs, nullptr);
	}

	vk::ShaderModule _vs;
	vk::ShaderModule _ps;
	vk::ShaderModule _ds;
	vk::ShaderModule _hs;
	vk::ShaderModule _gs;

	uint8_t _type = 0;
};

struct st_vk_texture : public st_texture, public st_vk_resource
{
	~st_vk_texture()
	{
		_device->destroyImageView(_view, nullptr);
		_device->freeMemory(_memory, nullptr);
		_device->destroyImage(_handle, nullptr);
	}

	vk::Image _handle;
	vk::ImageView _view;
	vk::DeviceMemory _memory;

	uint32_t _width = 0;
	uint32_t _height = 0;
	uint32_t _levels = 1;
	e_st_format _format = st_format_unknown;
	e_st_texture_usage_flags _usage = static_cast<e_st_texture_usage_flags>(0);
	e_st_texture_state _state = st_texture_state_common;
};

struct st_vk_texture_view : public st_texture_view, public st_vk_resource
{
	~st_vk_texture_view() { _device->destroyImageView(_view, nullptr); }

	vk::ImageView _view;
};

struct st_vk_vertex_format : public st_vertex_format, public st_vk_resource
{
	std::vector<vk::VertexInputBindingDescription> _binding_descs;
	std::vector<vk::VertexInputAttributeDescription> _attribute_descs;
	vk::PipelineVertexInputStateCreateInfo _input_layout;
};
