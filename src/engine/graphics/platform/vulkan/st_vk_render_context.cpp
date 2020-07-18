/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_context.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_texture.h>

#include <system/st_window.h>

#include <iostream>
#include <vector>

st_vk_render_context* st_vk_render_context::_this = nullptr;

st_vk_render_context::st_vk_render_context(const st_window* window)
{
	std::vector<const char*> layer_names;
	std::vector<const char*> extension_names;

	// List all the layers available in the instance.
	uint32_t layer_count;
	VK_VALIDATE(vk::enumerateInstanceLayerProperties(&layer_count, nullptr));

	std::vector<vk::LayerProperties> layers;
	layers.resize(layer_count);
	VK_VALIDATE(vk::enumerateInstanceLayerProperties(&layer_count, layers.data()));

	for (auto& layer : layers)
	{
#if _DEBUG
		std::cout << "Layer: " << layer.layerName << "\tImplementation: " << layer.implementationVersion << std::endl;
		std::cout << "\t" << layer.description << std::endl;
#endif

		// Naively enable all validation layers.
		if (strstr(layer.layerName, "validation") != nullptr)
		{
			layer_names.push_back(layer.layerName);
		}
	}

	// List all the extensions available in the instance.
	uint32_t extension_count;
	VK_VALIDATE(vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr));

	std::vector<vk::ExtensionProperties> extensions;
	extensions.resize(extension_count);
	VK_VALIDATE(vk::enumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()));

	for (auto& extension : extensions)
	{
#if _DEBUG
		std::cout << "Instance Extension: " << extension.extensionName <<
			"\tSpec: " << extension.specVersion << std::endl;
#endif

		// Naively enable all instance extensions.
		extension_names.push_back(extension.extensionName);
	}

	// Create the per-application instance object.
	auto app_info = vk::ApplicationInfo()
		.setPApplicationName("Stratos Renderer")
		.setPEngineName("Stratos")
		.setEngineVersion(1);

	auto create_info = vk::InstanceCreateInfo()
		.setPApplicationInfo(&app_info)
		.setEnabledLayerCount(layer_names.size())
		.setPpEnabledLayerNames(layer_names.data())
		.setEnabledExtensionCount(extension_names.size())
		.setPpEnabledExtensionNames(extension_names.data());

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

	layer_names.clear();
	extension_names.clear();

	// List all the device layers available.
	VK_VALIDATE(_gpu.enumerateDeviceLayerProperties(&layer_count, nullptr));

	layers.clear();
	layers.resize(layer_count);
	VK_VALIDATE(_gpu.enumerateDeviceLayerProperties(&layer_count, layers.data()));

	for (auto& layer : layers)
	{
#if _DEBUG
		std::cout << "Device Layer: " << layer.layerName << "\tImplementation: " << layer.implementationVersion << std::endl;
		std::cout << "\t" << layer.description << std::endl;
#endif
	}

	// List all the device extensions available.
	VK_VALIDATE(_gpu.enumerateDeviceExtensionProperties(nullptr, &extension_count, nullptr));

	extensions.clear();
	extensions.resize(extension_count);
	VK_VALIDATE(_gpu.enumerateDeviceExtensionProperties(nullptr, &extension_count, extensions.data()));

	for (auto& extension : extensions)
	{
#if _DEBUG
		std::cout << "Device Extension: " << extension.extensionName <<
			"\tSpec: " << extension.specVersion << std::endl;
#endif

		extension_names.push_back(extension.extensionName);
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
		.setEnabledExtensionCount(extension_names.size())
		.setPpEnabledExtensionNames(extension_names.data())
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
	VK_VALIDATE(_device.createFence(&fence_info, nullptr, &_acquire_fence));

	// Create the swap chain.
	vk::Win32SurfaceCreateInfoKHR win32_surface_info = vk::Win32SurfaceCreateInfoKHR()
		.setHinstance(GetModuleHandle(NULL))
		.setHwnd(window->get_window_handle());

	VK_VALIDATE(_instance.createWin32SurfaceKHR(&win32_surface_info, nullptr, &_window_surface));

	vk::Bool32 surface_support = false;
	VK_VALIDATE(_gpu.getSurfaceSupportKHR(_queue_family_index, _window_surface, &surface_support));
	assert(surface_support);

	vk::SwapchainCreateInfoKHR swap_chain_info = vk::SwapchainCreateInfoKHR()
		.setSurface(_window_surface)
		.setMinImageCount(k_backbuffer_count)
		.setImageFormat(vk::Format::eR8G8B8A8Srgb)
		.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
		.setImageExtent(vk::Extent2D(window->get_width(), window->get_height()))
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&_queue_family_index)
		.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(vk::PresentModeKHR::eImmediate)
		.setClipped(false);

	VK_VALIDATE(_device.createSwapchainKHR(&swap_chain_info, nullptr, &_swap_chain));

	uint32_t backbuffer_count;
	VK_VALIDATE(_device.getSwapchainImagesKHR(_swap_chain, &backbuffer_count, nullptr));
	assert(backbuffer_count = k_backbuffer_count);
	VK_VALIDATE(_device.getSwapchainImagesKHR(_swap_chain, &backbuffer_count, &_backbuffers[0]));

	// Create the generic upload buffer.
	create_buffer(16 * 1024 * 1024, e_st_buffer_usage::transfer_source | e_st_buffer_usage::transfer_dest, _upload_buffer);

	// Set up the descriptor set layout. This is akin to the root signature in D3D12.
	std::vector<vk::DescriptorSetLayoutBinding> bindings;
	bindings.push_back(vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorCount(4)
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment));
	bindings.push_back(vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorCount(4)
		.setDescriptorType(vk::DescriptorType::eSampler)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment));
	bindings.push_back(vk::DescriptorSetLayoutBinding()
		.setBinding(0)
		.setDescriptorCount(2)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eAll));
	bindings.push_back(vk::DescriptorSetLayoutBinding()
		.setBinding(1)
		.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eStorageBuffer)
		.setStageFlags(vk::ShaderStageFlagBits::eFragment));

	std::vector<vk::DescriptorSetLayoutCreateInfo> layout_infos;
	layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(bindings.data()));
	layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(&bindings.data()[1]));
	layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(&bindings.data()[2]));
	layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
		.setBindingCount(1)
		.setPBindings(&bindings.data()[3]));

	VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[0], nullptr, &_descriptor_layouts[0]));
	VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[1], nullptr, &_descriptor_layouts[1]));
	VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[2], nullptr, &_descriptor_layouts[2]));
	VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[3], nullptr, &_descriptor_layouts[3]));

	vk::PipelineLayoutCreateInfo pipeline_layout_info = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount(4)
		.setPSetLayouts(&_descriptor_layouts[0])
		.setPushConstantRangeCount(0);

	VK_VALIDATE(_device.createPipelineLayout(&pipeline_layout_info, nullptr, &_pipeline_layout));

	// Create the descriptor pool.
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(512));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(64));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampler)
		.setDescriptorCount(4));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(16));

	vk::DescriptorPoolCreateInfo pool_info = vk::DescriptorPoolCreateInfo()
		.setMaxSets(1024)
		.setPoolSizeCount(pool_sizes.size())
		.setPPoolSizes(pool_sizes.data());

	VK_VALIDATE(_device.createDescriptorPool(&pool_info, nullptr, &_descriptor_pool));

	_this = this;

	// Create the faux backbuffer target.
	_present_target = std::make_unique<st_render_texture>(
		window->get_width(),
		window->get_height(),
		st_format_r8g8b8a8_unorm,
		e_st_texture_usage::color_target | e_st_texture_usage::copy_source,
		st_vec4f{ 0.0f, 0.0f, 0.0f, 1.0f });
}

st_vk_render_context::~st_vk_render_context()
{
	_queue.waitIdle();
	_device.waitIdle();

	_present_target = nullptr;

	_device.destroySwapchainKHR(_swap_chain, nullptr);
	_instance.destroySurfaceKHR(_window_surface, nullptr);

	_device.destroyDescriptorPool(_descriptor_pool, nullptr);
	_device.destroyPipelineLayout(_pipeline_layout, nullptr);
	_device.destroyDescriptorSetLayout(_descriptor_layouts[0], nullptr);
	_device.destroyDescriptorSetLayout(_descriptor_layouts[1], nullptr);
	_device.destroyDescriptorSetLayout(_descriptor_layouts[2], nullptr);
	_device.destroyDescriptorSetLayout(_descriptor_layouts[3], nullptr);
	_device.destroyBuffer(_upload_buffer, nullptr);
	_device.destroyFence(_fence, nullptr);
	_device.destroyFence(_acquire_fence, nullptr);
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
}

void st_vk_render_context::transition_backbuffer_to_target()
{
	// Transition the present target to the optimal layout for rendering.
	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(1);

	vk::ImageMemoryBarrier barriers[] =
	{
		vk::ImageMemoryBarrier()
		.setImage(_present_target->get_resource())
		.setOldLayout(vk::ImageLayout::eTransferSrcOptimal)
		.setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setSubresourceRange(range),
	};
	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		std::size(barriers),
		barriers);
}

void st_vk_render_context::transition_backbuffer_to_present()
{
	// Transition the present target to the optimal layout for copy.
	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(1);

	vk::ImageMemoryBarrier barriers[] =
	{
		vk::ImageMemoryBarrier()
		.setImage(_present_target->get_resource())
		.setOldLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
		.setSubresourceRange(range),
	};
	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		barriers);
}

void st_vk_render_context::swap()
{
	// Copy the present target to the backbuffer.
	uint32_t backbuffer_index;
	VK_VALIDATE(_device.acquireNextImageKHR(
		_swap_chain,
		std::numeric_limits<uint64_t>::max(),
		vk::Semaphore(nullptr),
		_acquire_fence,
		&backbuffer_index));

	VK_VALIDATE(_device.waitForFences(1, &_acquire_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_acquire_fence));
	
	// Transition the backbuffer images to the optimal layout for the copy.
	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(1);

	vk::ImageMemoryBarrier barriers[] = 
	{
		vk::ImageMemoryBarrier()
			.setImage(_backbuffers[backbuffer_index])
			.setOldLayout(vk::ImageLayout::ePresentSrcKHR)
			.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
			.setSubresourceRange(range),
	};
	_command_buffer.pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		barriers);

	vk::ImageSubresourceLayers subresource = vk::ImageSubresourceLayers()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setMipLevel(0);

	vk::ImageCopy region = vk::ImageCopy()
		.setExtent(vk::Extent3D(_present_target->get_width(), _present_target->get_height(), 1))
		.setSrcSubresource(subresource)
		.setDstSubresource(subresource);

	_command_buffer.copyImage(
		_present_target->get_resource(),
		vk::ImageLayout::eTransferSrcOptimal,
		_backbuffers[backbuffer_index],
		vk::ImageLayout::eTransferDstOptimal,
		1,
		&region);

	barriers[0] = vk::ImageMemoryBarrier()
		.setImage(_backbuffers[backbuffer_index])
		.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
		.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
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
		barriers);

	_command_buffer.end();

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(&_command_buffer);

	vk::Result result = _device.getFenceStatus(_fence);
	VK_VALIDATE(_queue.submit(1, &submit_info, _fence));

	vk::PresentInfoKHR present_info = vk::PresentInfoKHR()
		.setSwapchainCount(1)
		.setPSwapchains(&_swap_chain)
		.setPImageIndices(&backbuffer_index);

	VK_VALIDATE(_queue.presentKHR(&present_info));

	// TODO: Better parallelization.
	VK_VALIDATE(_device.waitForFences(1, &_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_fence));
}

void st_vk_render_context::create_texture(
	uint32_t width,
	uint32_t height,
	uint32_t mip_count,
	e_st_format format,
	e_st_texture_usage_flags usage,
	vk::Image& resource)
{
	vk::ImageUsageFlags flags;

	if (usage & e_st_texture_usage::copy_source) flags |= vk::ImageUsageFlagBits::eTransferSrc;
	if (usage & e_st_texture_usage::copy_dest) flags |= vk::ImageUsageFlagBits::eTransferDst;
	if (usage & e_st_texture_usage::sampled) flags |= vk::ImageUsageFlagBits::eSampled;
	if (usage & e_st_texture_usage::storage) flags |= vk::ImageUsageFlagBits::eStorage;
	if (usage & e_st_texture_usage::color_target) flags |= vk::ImageUsageFlagBits::eColorAttachment;
	if (usage & e_st_texture_usage::depth_target) flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
	if (usage & e_st_texture_usage::transient_target) flags |= vk::ImageUsageFlagBits::eTransientAttachment;
	if (usage & e_st_texture_usage::input_target) flags |= vk::ImageUsageFlagBits::eInputAttachment;

	vk::ImageCreateInfo create_info = vk::ImageCreateInfo()
		.setArrayLayers(1)
		.setImageType(vk::ImageType::e2D)
		.setFormat((vk::Format)format)
		.setExtent(vk::Extent3D(width, height, 1))
		.setMipLevels(mip_count)
		.setUsage(flags)
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
}

void st_vk_render_context::destroy_texture(vk::Image& resource)
{
	_device.destroyImage(resource, nullptr);
}

void st_vk_render_context::create_texture_view(st_vk_texture* texture, vk::ImageView& resource)
{
	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (texture->get_format() == st_format_d16_unorm ||
		texture->get_format() == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (texture->get_format() == st_format_d24_unorm_s8_uint)
	{
		aspect = vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil;
	}

	vk::ImageSubresourceRange subresource_range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseMipLevel(0)
		.setLevelCount(texture->get_levels())
		.setBaseArrayLayer(0)
		.setLayerCount(1);

	vk::ImageViewCreateInfo create_info = vk::ImageViewCreateInfo()
		.setFormat(vk::Format(texture->get_format()))
		.setImage(texture->get_resource())
		.setViewType(vk::ImageViewType::e2D)
		.setSubresourceRange(subresource_range);

	_device.createImageView(&create_info, nullptr, &resource);
}

void st_vk_render_context::destroy_texture_view(vk::ImageView& resource)
{
	_device.destroyImageView(resource, nullptr);
}

void st_vk_render_context::upload_texture(st_vk_texture* texture, void* data)
{
	begin_loading();

	std::vector<vk::BufferImageCopy> regions;
	uint64_t offset = 0;
	for (int i = 0; i < texture->get_levels(); ++i)
	{
		uint32_t level_width = texture->get_width() >> i;
		uint32_t level_height = texture->get_height() >> i;

		size_t row_bytes;
		size_t num_bytes;
		get_surface_info(
			level_width,
			level_height,
			texture->get_format(),
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

	_command_buffer.copyBufferToImage(_upload_buffer, texture->get_resource(), vk::ImageLayout::eTransferDstOptimal, texture->get_levels(), regions.data());

	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(vk::ImageAspectFlagBits::eColor)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(texture->get_levels());

	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setImage(texture->get_resource())
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

void st_vk_render_context::create_descriptor_set(e_st_descriptor_slot slot, vk::DescriptorSet* set)
{
	vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(_descriptor_pool)
		.setDescriptorSetCount(1)
		.setPSetLayouts(&_descriptor_layouts[slot]);

	VK_VALIDATE(_device.allocateDescriptorSets(&allocate_info, set));
}

void st_vk_render_context::destroy_descriptor_set(vk::DescriptorSet& set)
{
	VK_VALIDATE(_device.freeDescriptorSets(_descriptor_pool, 1, &set));
}

void st_vk_render_context::create_sampler(vk::Sampler& sampler)
{
	vk::SamplerCreateInfo create_info = vk::SamplerCreateInfo()
		.setMagFilter(vk::Filter::eLinear)
		.setMinFilter(vk::Filter::eLinear)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear)
		.setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
		.setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
		.setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
		.setMipLodBias(0.0f)
		.setAnisotropyEnable(false)
		.setCompareEnable(false)
		.setMinLod(0)
		.setMaxLod(15.0f)
		.setUnnormalizedCoordinates(false);

	VK_VALIDATE(_device.createSampler(&create_info, nullptr, &sampler));
}

void st_vk_render_context::destroy_sampler(vk::Sampler& sampler)
{
	_device.destroySampler(sampler, nullptr);
}

#endif
