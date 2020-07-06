/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <iostream>
#include <vector>

#if defined(ST_GRAPHICS_API_VULKAN)

st_vk_render_context* st_vk_render_context::_this = nullptr;

st_vk_render_context::st_vk_render_context(const class st_window* window)
{
	std::vector<const char*> layer_names;
#if _DEBUG
	layer_names.push_back("VK_LAYER_KHRONOS_validation");
#endif

	// Create the per-application instance object.
	auto app_info = vk::ApplicationInfo()
		.setPApplicationName("Stratos Renderer")
		.setPEngineName("Stratos")
		.setEngineVersion(1);

	auto create_info = vk::InstanceCreateInfo()
		.setPApplicationInfo(&app_info)
		.setEnabledLayerCount(layer_names.size())
		.setPpEnabledLayerNames(layer_names.data())
		.setEnabledExtensionCount(0);

	VK_VALIDATE(vk::createInstance(&create_info, nullptr, &_instance));
	

	// Obtain the list of physical devices.
	uint32_t device_count;
	VK_VALIDATE(_instance.enumeratePhysicalDevices(&device_count, nullptr));

	std::vector<vk::PhysicalDevice> devices;
	devices.resize(device_count);
	VK_VALIDATE(_instance.enumeratePhysicalDevices(&device_count, devices.data()));

	for (auto& device : devices)
	{
		_gpu = device;

		vk::PhysicalDeviceProperties properties;
		device.getProperties(&properties);

		std::cout << "Found physical device: " << properties.deviceName << std::endl;

		// Just pick the first discrete GPU that we find.
		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		{
			break;
		}
	}

	// Query the device memory properties.
	vk::PhysicalDeviceMemoryProperties memory_props;
	_gpu.getMemoryProperties(&memory_props);

	// Fill out the device memory type indices.
	for (int i = 0; i < memory_props.memoryTypeCount; ++i)
	{
		if (memory_props.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
		{
			_device_memory_index = i;
			break;
		}
	}

	// For the physical device we chose, query the queue families it supports.
	uint32_t queue_family_count;
	_gpu.getQueueFamilyProperties(&queue_family_count, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_family_properties;
	queue_family_properties.resize(queue_family_count);
	_gpu.getQueueFamilyProperties(&queue_family_count, queue_family_properties.data());

	// Enumerate the queues, and select the one that supports graphics.
	uint32_t _queue_family_index = UINT_MAX;
	for (auto& properties : queue_family_properties)
	{
		std::cout << "Queue family: " << vk::to_string(properties.queueFlags) << std::endl;
		std::cout << "\tCount: " << properties.queueCount << std::endl;

		if (_queue_family_index > queue_family_count &&
			properties.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			_queue_family_index = &properties - &queue_family_properties[0];
		}
	}

	// Create a logical device object to correspond to our GPU.
	float const priorities[1] = { 0.0f };

	vk::DeviceQueueCreateInfo queues = vk::DeviceQueueCreateInfo()
		.setQueueFamilyIndex(_queue_family_index)
		.setQueueCount(1)
		.setPQueuePriorities(priorities);

	vk::DeviceCreateInfo device_info = vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&queues)
		.setEnabledLayerCount(0)
		.setPpEnabledLayerNames(nullptr)
		.setEnabledExtensionCount(0)
		.setPpEnabledExtensionNames(nullptr)
		.setPEnabledFeatures(nullptr);

	VK_VALIDATE(_gpu.createDevice(&device_info, nullptr, &_device));

	_device.getQueue(_queue_family_index, 0, &_queue);

	vk::CommandPoolCreateInfo command_pool_info = vk::CommandPoolCreateInfo()
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
		.setQueueFamilyIndex(_queue_family_index);

	VK_VALIDATE(_device.createCommandPool(&command_pool_info, nullptr, &_command_pool));

	vk::CommandBufferAllocateInfo command_buffer_info = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1)
		.setCommandPool(_command_pool)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	VK_VALIDATE(_device.allocateCommandBuffers(&command_buffer_info, &_command_buffer));

	vk::FenceCreateInfo fence_info = vk::FenceCreateInfo();

	VK_VALIDATE(_device.createFence(&fence_info, nullptr, &_fence));

	create_buffer(16 * 1024 * 1024, e_st_buffer_usage::transfer_source | e_st_buffer_usage::transfer_dest, _upload_buffer);

	_this = this;
}

st_vk_render_context::~st_vk_render_context()
{
	_queue.waitIdle();
	_device.waitIdle();

	_device.destroyBuffer(_upload_buffer, nullptr);
	_device.destroyFence(_fence, nullptr);
	_device.freeCommandBuffers(_command_pool, 1, &_command_buffer);
	_device.destroyCommandPool(_command_pool, nullptr);
	_device.destroy(nullptr);

	_instance.destroy(nullptr);
}

void st_vk_render_context::begin_loading()
{
	end_frame();
	begin_frame();
}

void st_vk_render_context::end_loading()
{
	end_frame();
	begin_frame();
}

void st_vk_render_context::begin_frame()
{
	vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	VK_VALIDATE(_command_buffer.begin(&begin_info));
}

void st_vk_render_context::end_frame()
{
	VK_VALIDATE(_command_buffer.end());
}

void st_vk_render_context::swap()
{
	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&_command_buffer);

	vk::Result result = _device.getFenceStatus(_fence);
	VK_VALIDATE(_queue.submit(1, &submit_info, _fence));

	// TODO: Better parallelization.
	VK_VALIDATE(_device.waitForFences(1, &_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_fence));
}

void st_vk_render_context::create_texture(
	uint32_t width,
	uint32_t height,
	uint32_t mip_count,
	e_st_format format,
	void* data,
	vk::Image& resource)
{
	begin_loading();

	vk::ImageCreateInfo create_info = vk::ImageCreateInfo()
		.setArrayLayers(1)
		.setImageType(vk::ImageType::e2D)
		.setFormat((vk::Format)format)
		.setExtent(vk::Extent3D(width, height, 1))
		.setMipLevels(mip_count)
		.setUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
		.setInitialLayout(vk::ImageLayout::ePreinitialized)
		.setTiling(vk::ImageTiling::eOptimal);

	VK_VALIDATE(_device.createImage(&create_info, nullptr, &resource));

	// Allocate memory for the image.
	vk::MemoryRequirements memory_reqs;
	_device.getImageMemoryRequirements(resource, &memory_reqs);

	vk::MemoryAllocateInfo allocate_info = vk::MemoryAllocateInfo()
		.setAllocationSize(memory_reqs.size)
		.setMemoryTypeIndex(_device_memory_index);

	vk::DeviceMemory memory;
	_device.allocateMemory(&allocate_info, nullptr, &memory);

	_device.bindImageMemory(resource, memory, 0);

	std::vector<vk::BufferImageCopy> regions;
	uint64_t offset = 0;
	for (int i = 0; i < mip_count; ++i)
	{
		uint32_t level_width = width >> i;
		uint32_t level_height = height >> i;

		size_t row_bytes;
		size_t num_bytes;
		get_surface_info(
			level_width,
			level_height,
			format,
			&num_bytes,
			&row_bytes,
			nullptr);

		_command_buffer.updateBuffer(_upload_buffer, offset, num_bytes, reinterpret_cast<char*>(data) + offset);
		offset += num_bytes;

		vk::ImageSubresourceLayers subresource = vk::ImageSubresourceLayers()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setLayerCount(1)
			.setMipLevel(i);

		vk::BufferImageCopy region = vk::BufferImageCopy()
			.setBufferImageHeight(level_height)
			.setBufferRowLength(row_bytes)
			.setBufferOffset(offset)
			.setImageExtent(vk::Extent3D(level_width, level_height, 1))
			.setImageOffset(vk::Offset3D(0, 0, 0))
			.setImageSubresource(subresource);

		regions.push_back(region);
	}

	_command_buffer.copyBufferToImage(_upload_buffer, resource, vk::ImageLayout::eTransferDstOptimal, mip_count, regions.data());

	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(mip_count);

	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setImage(resource)
		.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
		.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
		.setSubresourceRange(range);
	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		&barrier);

	end_loading();
}

void st_vk_render_context::destroy_texture(vk::Image& resource)
{
	_device.destroyImage(resource, nullptr);
}

void st_vk_render_context::create_buffer(size_t size, e_st_buffer_usage_flags usage, vk::Buffer& resource)
{
	vk::BufferUsageFlags flags;

	if (usage & e_st_buffer_usage::index) flags |= vk::BufferUsageFlagBits::eIndexBuffer;
	if (usage & e_st_buffer_usage::indirect) flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
	if (usage & e_st_buffer_usage::storage) flags |= vk::BufferUsageFlagBits::eStorageBuffer;
	if (usage & e_st_buffer_usage::storage_texel) flags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
	if (usage & e_st_buffer_usage::transfer_dest) flags |= vk::BufferUsageFlagBits::eTransferDst;
	if (usage & e_st_buffer_usage::transfer_source) flags |= vk::BufferUsageFlagBits::eTransferSrc;
	if (usage & e_st_buffer_usage::uniform) flags |= vk::BufferUsageFlagBits::eUniformBuffer;
	if (usage & e_st_buffer_usage::uniform_texel) flags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
	if (usage & e_st_buffer_usage::vertex) flags |= vk::BufferUsageFlagBits::eVertexBuffer;

	vk::BufferCreateInfo buffer_info = vk::BufferCreateInfo()
		.setUsage(flags)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&_queue_family_index)
		.setSize(size);

	VK_VALIDATE(_device.createBuffer(&buffer_info, nullptr, &resource));

	// Allocate memory for the buffer.
	vk::MemoryRequirements memory_reqs;
	_device.getBufferMemoryRequirements(resource, &memory_reqs);

	vk::MemoryAllocateInfo allocate_info = vk::MemoryAllocateInfo()
		.setAllocationSize(memory_reqs.size)
		.setMemoryTypeIndex(_device_memory_index);

	vk::DeviceMemory memory;
	_device.allocateMemory(&allocate_info, nullptr, &memory);

	_device.bindBufferMemory(resource, memory, 0);
}

void st_vk_render_context::update_buffer(vk::Buffer& resource, size_t offset, size_t num_bytes, const void* data)
{
	_command_buffer.updateBuffer(resource, offset, num_bytes, data);
}

void st_vk_render_context::destroy_buffer(vk::Buffer& resource)
{
	_device.destroyBuffer(resource, nullptr);
}

#endif
