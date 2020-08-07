/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_resource_table.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_render_context.h>
#include <graphics/st_buffer.h>
#include <graphics/st_constant_buffer.h>
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

	for (auto& sampler : _sampler_resources)
	{
		context->destroy_sampler(sampler);
	}
	_sampler_resources.clear();

	context->destroy_descriptor_set(_textures);
	context->destroy_descriptor_set(_buffers);
	context->destroy_descriptor_set(_constants);
	context->destroy_descriptor_set(_samplers);
}

void st_vk_resource_table::set_constant_buffers(uint32_t count, st_constant_buffer** cbs)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	std::vector<vk::DescriptorBufferInfo> infos;
	for (int i = 0; i < count; ++i)
	{
		infos.emplace_back() = vk::DescriptorBufferInfo()
			.setBuffer(cbs[i]->get_resource())
			.setOffset(0)
			.setRange(vk::DeviceSize(VK_WHOLE_SIZE));
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(count)
		.setDstSet(_constants)
		.setDstBinding(0)
		.setPBufferInfo(infos.data());

	device->updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_resource_table::set_textures(uint32_t count, st_texture** textures)
{
	vk::Device* device = st_vk_render_context::get()->get_device();

	std::vector<vk::DescriptorImageInfo> images;
	for (int i = 0; i < count; ++i)
	{
		st_vk_render_context::get()->create_sampler(_sampler_resources.emplace_back());

		images.emplace_back() = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(textures[i]->get_resource_view())
			.setSampler(_sampler_resources.back());
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(count)
		.setDstSet(_textures)
		.setDstBinding(0)
		.setPImageInfo(images.data());

	device->updateDescriptorSets(1, &write_set, 0, nullptr);

	write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampler)
		.setDescriptorCount(count)
		.setDstSet(_samplers)
		.setDstBinding(0)
		.setPImageInfo(images.data());

	device->updateDescriptorSets(1, &write_set, 0, nullptr);
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

	device->updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_resource_table::bind(st_vk_render_context* context)
{
	context->set_shader_resource_table(_textures);
	context->set_sampler_table(_samplers);
	context->set_buffer_table(_buffers);
	context->set_constant_buffer_table(_constants);
}

#endif
