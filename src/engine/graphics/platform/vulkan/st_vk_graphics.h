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

struct st_vk_buffer : public st_buffer
{
	uint32_t _count;
	e_st_buffer_usage_flags _usage;
	size_t _element_size;

	vk::Buffer _buffer;
	vk::DeviceMemory _memory;
};

struct st_vk_buffer_view : public st_buffer_view
{
	vk::BufferView _view;
};

struct st_vk_geometry : public st_geometry
{
	std::unique_ptr<st_buffer> _vertex_buffer;
	std::unique_ptr<st_buffer> _index_buffer;

	std::unique_ptr<st_buffer_view> _vertex_buffer_view;
	std::unique_ptr<st_buffer_view> _index_buffer_view;

	uint32_t _index_count = 0;
};

struct st_vk_pipeline : public st_pipeline
{
	vk::Pipeline _pipeline;
};

struct st_vk_render_pass : public st_render_pass
{
	vk::RenderPass _render_pass;
	std::unique_ptr<class st_vk_framebuffer> _framebuffer;
	vk::Viewport _viewport;
};

struct st_vk_resource_table : public st_resource_table
{
	vk::DescriptorSet _textures;
	vk::DescriptorSet _constants;
	vk::DescriptorSet _buffers;
	vk::DescriptorSet _samplers;

	std::vector<vk::Sampler> _sampler_resources;
};

struct st_vk_shader : public st_shader
{
	vk::ShaderModule _vs;
	vk::ShaderModule _ps;
	vk::ShaderModule _ds;
	vk::ShaderModule _hs;
	vk::ShaderModule _gs;

	uint8_t _type = 0;
};

struct st_vk_texture : public st_texture
{
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

struct st_vk_texture_view : public st_texture_view
{
	vk::ImageView _view;
};

struct st_vk_vertex_format : public st_vertex_format
{
	std::vector<vk::VertexInputBindingDescription> _binding_descs;
	std::vector<vk::VertexInputAttributeDescription> _attribute_descs;
	vk::PipelineVertexInputStateCreateInfo _input_layout;
};
