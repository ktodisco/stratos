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

#include <vector>

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
	const st_vk_buffer* _buffer;
	uint32_t _first_element;
	uint32_t _element_count;
};

struct st_vk_framebuffer : public st_framebuffer, public st_vk_resource
{
	~st_vk_framebuffer()
	{
		_device->destroyFramebuffer(_framebuffer, nullptr);
		_targets.clear();
	}

	vk::Framebuffer _framebuffer;
	std::vector<st_texture*> _targets;
	st_texture* _depth_stencil = nullptr;

	uint32_t _width;
	uint32_t _height;
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
	vk::Viewport _viewport;
};

struct st_vk_resource_table : public st_resource_table, public st_vk_resource
{
	~st_vk_resource_table()
	{
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
	vk::DescriptorSet _uavs;

	uint32_t _texture_count = 0;
	uint32_t _constant_count = 0;
	uint32_t _buffer_count = 0;
	uint32_t _sampler_count = 0;
	uint32_t _uav_count = 0;

	std::vector<const struct st_vk_sampler*> _sampler_resources;

	vk::DescriptorPool* _pool;
};

struct st_vk_sampler : public st_sampler, public st_vk_resource
{
	~st_vk_sampler()
	{
		_device->destroySampler(_sampler, nullptr);
	}

	vk::Sampler _sampler;
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
		_device->destroyShaderModule(_cs, nullptr);
	}

	vk::ShaderModule _vs;
	vk::ShaderModule _ps;
	vk::ShaderModule _ds;
	vk::ShaderModule _hs;
	vk::ShaderModule _gs;
	vk::ShaderModule _cs;

	uint8_t _type = 0;
};

struct st_vk_swap_chain : public st_swap_chain, public st_vk_resource
{
	~st_vk_swap_chain()
	{
		_device->destroySwapchainKHR(_swap_chain, nullptr);
		_instance->destroySurfaceKHR(_window_surface, nullptr);

		for (uint32_t i = 0; i < _backbuffers.size(); ++i)
			free(_backbuffers[i]);
	}

	vk::Instance* _instance;
	vk::SurfaceKHR _window_surface;
	vk::SwapchainKHR _swap_chain;
	// The Vulkan swap chain object owns the backbuffer textures, so raw pointers are used here.
	std::vector<struct st_texture*> _backbuffers;
	std::vector<std::unique_ptr<st_texture_view>> _backbuffer_views;
};

struct st_vk_texture : public st_texture, public st_vk_resource
{
	~st_vk_texture()
	{
		_device->freeMemory(_memory, nullptr);
		_device->destroyImage(_handle, nullptr);
	}

	vk::Image _handle;
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
