/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_resource_table.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/st_buffer.h>
#include <graphics/st_texture.h>

st_vk_resource_table::st_vk_resource_table()
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->create_descriptor_set(st_descriptor_slot_textures, &_textures);
	context->create_descriptor_set(st_descriptor_slot_buffers, &_buffers);
	context->create_descriptor_set(st_descriptor_slot_constants, &_constants);
	context->create_descriptor_set(st_descriptor_slot_samplers, &_samplers);
}

st_vk_resource_table::~st_vk_resource_table()
{
	st_vk_render_context* context = st_vk_render_context::get();

	context->destroy_descriptor_set(_textures);
	context->destroy_descriptor_set(_buffers);
	context->destroy_descriptor_set(_constants);
	context->destroy_descriptor_set(_samplers);
}

void st_vk_resource_table::set_constant_buffers(uint32_t count, st_constant_buffer** cbs)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	// TODO.
}

void st_vk_resource_table::set_textures(uint32_t count, st_texture** textures)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	std::vector<vk::DescriptorImageInfo> images;
	for (int i = 0; i < count; ++i)
	{
		images.emplace_back() = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(textures[i]->get_resource_view())
			// TODO: Samplers.
			.setSampler(vk::Sampler(nullptr));
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(count)
		.setDstSet(_textures)
		.setDstBinding(0)
		.setPImageInfo(images.data());
}

void st_vk_resource_table::set_buffers(uint32_t count, st_buffer** buffers)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	std::vector<vk::DescriptorBufferInfo> infos;
	for (int i = 0; i < count; ++i)
	{
		infos.emplace_back() = vk::DescriptorBufferInfo()
			.setBuffer(buffers[i]->get_resource())
			.setOffset(0)
			.setRange(vk::DeviceSize(VK_WHOLE_SIZE));
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(count)
		.setDstSet(_buffers)
		.setDstBinding(0)
		.setPBufferInfo(infos.data());
}

#endif
