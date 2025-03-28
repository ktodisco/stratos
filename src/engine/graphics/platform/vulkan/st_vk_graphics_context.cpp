/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics_context.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_conversion.h>

#include <graphics/geometry/st_vertex_attribute.h>

#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader_manager.h>

#include <system/st_window.h>

#include <iostream>
#include <fstream>
#include <vector>

extern char g_root_path[256];

extern vk::DispatchLoaderDynamic vk::defaultDispatchLoaderDynamic;

st_vk_graphics_context::st_vk_graphics_context(const st_window* window)
{
	_vk_library = LoadLibrary("vulkan-1.dll");
	PFN_vkGetInstanceProcAddr fp = (PFN_vkGetInstanceProcAddr)GetProcAddress((HMODULE)_vk_library, "vkGetInstanceProcAddr");

	vk::defaultDispatchLoaderDynamic.init(fp);

	uint32_t api_version;
	VK_VALIDATE(vk::enumerateInstanceVersion(&api_version));

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

#if 0
		// TODO: This crashes the application when launching through Renderdoc.
		if (strstr(layer.layerName, "RENDERDOC") != nullptr)
		{
			HMODULE rdoc = GetModuleHandle("renderdoc.dll");
			if (rdoc)
			{
				layer_names.push_back(layer.layerName);
			}
		}
#endif

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
		.setEngineVersion(1)
		.setApiVersion(api_version);

	auto create_info = vk::InstanceCreateInfo()
		.setPApplicationInfo(&app_info)
		.setEnabledLayerCount(layer_names.size())
		.setPpEnabledLayerNames(layer_names.data())
		.setEnabledExtensionCount(extension_names.size())
		.setPpEnabledExtensionNames(extension_names.data());

	VK_VALIDATE(vk::createInstance(&create_info, nullptr, &_instance));

	vk::defaultDispatchLoaderDynamic.init(_instance);

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

		if (strcmp(extension.extensionName, "VK_EXT_debug_marker") == 0)
		{
			_has_markers = true;
		}
	}

	auto bda_extension = std::find_if(extension_names.begin(), extension_names.end(),
		[](const char* str) { return strcmp(str, "VK_EXT_buffer_device_address") == 0; });
	if (bda_extension != extension_names.end())
	{
		auto khr_extension = std::find_if(extension_names.begin(), extension_names.end(),
			[](const char* str) { return strcmp(str, "VK_KHR_buffer_device_address") == 0; });
		if (khr_extension != extension_names.end())
			extension_names.erase(khr_extension);
	}

	// Query the device capabilities.
	_gpu.getProperties(&_caps);

	// Query the device memory properties.
	vk::PhysicalDeviceMemoryProperties memory_props;
	_gpu.getMemoryProperties(&memory_props);

	// Fill out the device memory type indices.
	for (int i = 0; i < memory_props.memoryTypeCount; ++i)
	{
		if (memory_props.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal &&
			_device_memory_index == UINT_MAX)
		{
			_device_memory_index = i;
		}

		if (memory_props.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostVisible &&
			memory_props.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eHostCoherent &&
			_mapped_memory_index == UINT_MAX)
		{
			_mapped_memory_index = i;
		}
	}

	// For the physical device we chose, query the queue families it supports.
	uint32_t queue_family_count;
	_gpu.getQueueFamilyProperties(&queue_family_count, nullptr);

	std::vector<vk::QueueFamilyProperties> queue_family_properties;
	queue_family_properties.resize(queue_family_count);
	_gpu.getQueueFamilyProperties(&queue_family_count, queue_family_properties.data());

	// Enumerate the queues, and select the one that supports graphics.
	for (auto& properties : queue_family_properties)
	{
		std::cout << "Queue family: " << vk::to_string(properties.queueFlags) << std::endl;
		std::cout << "\tCount: " << properties.queueCount << std::endl;

		if (_queue_family_index > queue_family_count &&
			properties.queueFlags & vk::QueueFlagBits::eGraphics &&
			properties.queueFlags & vk::QueueFlagBits::eCompute)
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

	VK_VALIDATE(_device.createCommandPool(&command_pool_info, nullptr, &_command_pools[st_command_buffer_graphics]));
	VK_VALIDATE(_device.createCommandPool(&command_pool_info, nullptr, &_command_pools[st_command_buffer_loading]));

	vk::CommandBufferAllocateInfo command_buffer_info = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1)
		.setCommandPool(_command_pools[st_command_buffer_graphics])
		.setLevel(vk::CommandBufferLevel::ePrimary);

	VK_VALIDATE(_device.allocateCommandBuffers(&command_buffer_info, &_command_buffers[st_command_buffer_graphics]));

	command_buffer_info.setCommandPool(_command_pools[st_command_buffer_loading]);
	VK_VALIDATE(_device.allocateCommandBuffers(&command_buffer_info, &_command_buffers[st_command_buffer_loading]));

	vk::FenceCreateInfo fence_info = vk::FenceCreateInfo();

	VK_VALIDATE(_device.createFence(&fence_info, nullptr, &_fence));
	VK_VALIDATE(_device.createFence(&fence_info, nullptr, &_acquire_fence));

	// Create the generic upload buffer.
	vk::BufferCreateInfo buffer_info = vk::BufferCreateInfo()
		.setUsage(vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&_queue_family_index)
		.setSize(128 * 1024 * 1024);

	VK_VALIDATE(_device.createBuffer(&buffer_info, nullptr, &_upload_buffer));

	// Allocate memory for the buffer.
	vk::MemoryRequirements memory_reqs;
	_device.getBufferMemoryRequirements(_upload_buffer, &memory_reqs);

	vk::MemoryAllocateInfo allocate_info = vk::MemoryAllocateInfo()
		.setAllocationSize(memory_reqs.size)
		.setMemoryTypeIndex(_mapped_memory_index);

	VK_VALIDATE(_device.allocateMemory(&allocate_info, nullptr, &_upload_buffer_memory));

	VK_VALIDATE(_device.bindBufferMemory(_upload_buffer, _upload_buffer_memory, 0));
	VK_VALIDATE(_device.mapMemory(_upload_buffer_memory, vk::DeviceSize(), memory_reqs.size, vk::MemoryMapFlags(), &_upload_buffer_head));
	_upload_buffer_offset = 0;

	// Set up the pipeline layouts. These are akin to root signatures in D3D12.
	// First the graphics layout.
	{
		std::vector<vk::DescriptorSetLayoutBinding> textureBindings;
		for (uint32_t i = 0; i < 8; ++i)
		{
			textureBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eSampledImage)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment));
		}
		std::vector<vk::DescriptorSetLayoutBinding> samplerBindings;
		for (uint32_t i = 0; i < 8; ++i)
		{
			samplerBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eSampler)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment));
		}
		std::vector<vk::DescriptorSetLayoutBinding> constantBindings;
		for (uint32_t i = 0; i < 2; ++i)
		{
			constantBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eAll));
		}
		std::vector<vk::DescriptorSetLayoutBinding> bufferBindings;
		for (uint32_t i = 0; i < 1; ++i)
		{
			bufferBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eStorageBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment));
		}

		std::vector<vk::DescriptorSetLayoutCreateInfo> layout_infos;
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(textureBindings.size())
			.setPBindings(textureBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(samplerBindings.size())
			.setPBindings(samplerBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(constantBindings.size())
			.setPBindings(constantBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(bufferBindings.size())
			.setPBindings(bufferBindings.data()));

		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[0], nullptr, &_graphics_layouts[0]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[1], nullptr, &_graphics_layouts[1]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[2], nullptr, &_graphics_layouts[2]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[3], nullptr, &_graphics_layouts[3]));

		vk::PipelineLayoutCreateInfo pipeline_layout_info = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(_countof(_graphics_layouts))
			.setPSetLayouts(&_graphics_layouts[0])
			.setPushConstantRangeCount(0);

		VK_VALIDATE(_device.createPipelineLayout(&pipeline_layout_info, nullptr, &_graphics_signature));
	}

	// Then compute.
	{
		std::vector<vk::DescriptorSetLayoutBinding> textureBindings;
		for (uint32_t i = 0; i < 8; ++i)
		{
			textureBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eSampledImage)
				.setStageFlags(vk::ShaderStageFlagBits::eCompute));
		}
		std::vector<vk::DescriptorSetLayoutBinding> samplerBindings;
		for (uint32_t i = 0; i < 8; ++i)
		{
			samplerBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eSampler)
				.setStageFlags(vk::ShaderStageFlagBits::eCompute));
		}
		std::vector<vk::DescriptorSetLayoutBinding> constantBindings;
		for (uint32_t i = 0; i < 2; ++i)
		{
			constantBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eCompute));
		}
		std::vector<vk::DescriptorSetLayoutBinding> imageBindings;
		for (uint32_t i = 0; i < 1; ++i)
		{
			imageBindings.push_back(vk::DescriptorSetLayoutBinding()
				.setBinding(i)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eStorageImage)
				.setStageFlags(vk::ShaderStageFlagBits::eCompute));
		}

		std::vector<vk::DescriptorSetLayoutCreateInfo> layout_infos;
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(textureBindings.size())
			.setPBindings(textureBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(samplerBindings.size())
			.setPBindings(samplerBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(constantBindings.size())
			.setPBindings(constantBindings.data()));
		layout_infos.push_back(vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(imageBindings.size())
			.setPBindings(imageBindings.data()));

		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[0], nullptr, &_compute_layouts[0]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[1], nullptr, &_compute_layouts[1]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[2], nullptr, &_compute_layouts[2]));
		VK_VALIDATE(_device.createDescriptorSetLayout(&layout_infos[3], nullptr, &_compute_layouts[3]));

		vk::PipelineLayoutCreateInfo pipeline_layout_info = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(_countof(_compute_layouts))
			.setPSetLayouts(&_compute_layouts[0])
			.setPushConstantRangeCount(0);

		VK_VALIDATE(_device.createPipelineLayout(&pipeline_layout_info, nullptr, &_compute_signature));
	}

	// Create the descriptor pool.
	std::vector<vk::DescriptorPoolSize> pool_sizes;
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(k_max_shader_resources));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(k_max_shader_resources));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eSampler)
		.setDescriptorCount(k_max_samplers));
	pool_sizes.push_back(vk::DescriptorPoolSize()
		.setType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(k_max_shader_resources));

	vk::DescriptorPoolCreateInfo pool_info = vk::DescriptorPoolCreateInfo()
		.setMaxSets(1024)
		.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
		.setPoolSizeCount(pool_sizes.size())
		.setPPoolSizes(pool_sizes.data());

	VK_VALIDATE(_device.createDescriptorPool(&pool_info, nullptr, &_descriptor_pool));

	for (uint32_t ds_itr = 0; ds_itr < k_max_frames; ++ds_itr)
	{
		// TODO: Arbitrary number. What's the best way to size this?
		_descriptor_set_pool[ds_itr].reserve(1024);
	}

	// Create dynamic geometry buffers.
	const uint32_t k_dynamic_buffer_count = 10000;
	{
		st_buffer_desc desc;
		desc._count = k_dynamic_buffer_count;
		desc._element_size = sizeof(st_vk_procedural_vertex);
		desc._usage = e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest;
		_dynamic_vertex_buffer = create_buffer(desc);
	}
	{
		st_buffer_desc desc;
		desc._count = k_dynamic_buffer_count;
		desc._element_size = sizeof(uint16_t);
		desc._usage = e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest;
		_dynamic_index_buffer = create_buffer(desc);
	}

	begin_frame();
}

st_vk_graphics_context::~st_vk_graphics_context()
{
	VK_VALIDATE(_queue.waitIdle());
	VK_VALIDATE(_device.waitIdle());

	_dynamic_index_buffer = nullptr;
	_dynamic_vertex_buffer = nullptr;

	_device.destroyDescriptorPool(_descriptor_pool, nullptr);
	_device.destroyPipelineLayout(_compute_signature, nullptr);
	_device.destroyPipelineLayout(_graphics_signature, nullptr);
	for (uint32_t i = 0; i < _countof(_compute_layouts); ++i)
		_device.destroyDescriptorSetLayout(_compute_layouts[i], nullptr);
	for (uint32_t i = 0; i < _countof(_graphics_layouts); ++i)
		_device.destroyDescriptorSetLayout(_graphics_layouts[i], nullptr);
	_device.freeMemory(_upload_buffer_memory, nullptr);
	_device.destroyBuffer(_upload_buffer, nullptr);
	_device.destroyFence(_fence, nullptr);
	_device.destroyFence(_acquire_fence, nullptr);
	_device.freeCommandBuffers(
		_command_pools[st_command_buffer_graphics],
		1, 
		&_command_buffers[st_command_buffer_graphics]);
	_device.freeCommandBuffers(
		_command_pools[st_command_buffer_loading],
		1,
		&_command_buffers[st_command_buffer_loading]);
	_device.destroyCommandPool(_command_pools[st_command_buffer_graphics], nullptr);
	_device.destroyCommandPool(_command_pools[st_command_buffer_loading], nullptr);
	_device.destroy(nullptr);

	_instance.destroy(nullptr);

	FreeLibrary((HMODULE)_vk_library);
}

void st_vk_graphics_context::set_pipeline(const st_pipeline* pipeline_)
{
	const st_vk_pipeline* pipeline = static_cast<const st_vk_pipeline*>(pipeline_);
	_command_buffers[st_command_buffer_graphics].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->_pipeline);
}

void st_vk_graphics_context::set_compute_pipeline(const st_pipeline* pipeline_)
{
	const st_vk_pipeline* pipeline = static_cast<const st_vk_pipeline*>(pipeline_);
	_command_buffers[st_command_buffer_graphics].bindPipeline(vk::PipelineBindPoint::eCompute, pipeline->_pipeline);
}

void st_vk_graphics_context::set_viewport(const st_viewport& viewport)
{
	// Flip the Vulkan viewport to match the coordinate systems of OpenGL and DirectX.
	vk::Viewport view = vk::Viewport()
		.setWidth(viewport._width)
		.setHeight(-viewport._height)
		.setX(viewport._x)
		.setY(viewport._height - viewport._y)
		.setMinDepth(viewport._min_depth)
		.setMaxDepth(viewport._max_depth);
	_command_buffers[st_command_buffer_graphics].setViewportWithCount(1, &view);
}

void st_vk_graphics_context::set_scissor(int left, int top, int right, int bottom)
{
	vk::Rect2D scissor = vk::Rect2D()
		.setOffset({ left, top })
		.setExtent({ uint32_t(right - left), uint32_t(bottom - top) });
	_command_buffers[st_command_buffer_graphics].setScissorWithCount(1, &scissor);
}

void st_vk_graphics_context::draw(const st_static_drawcall& drawcall)
{
	const st_vk_buffer* vertex_buffer = static_cast<const st_vk_buffer*>(drawcall._vertex_buffer);
	const st_vk_buffer* index_buffer = static_cast<const st_vk_buffer*>(drawcall._index_buffer);

	vk::DeviceSize offset = vk::DeviceSize(0);
	_command_buffers[st_command_buffer_graphics].bindVertexBuffers(0, 1, &vertex_buffer->_buffer, &offset);
	_command_buffers[st_command_buffer_graphics].bindIndexBuffer(index_buffer->_buffer, 0, vk::IndexType::eUint16);

	_command_buffers[st_command_buffer_graphics].drawIndexed(
		drawcall._index_count,
		1,
		drawcall._index_offset,
		drawcall._vertex_offset,
		0);
}

void st_vk_graphics_context::draw(const struct st_dynamic_drawcall& drawcall)
{
	// TODO: Dynamic buffer limit checking.

	uint8_t* buffer_begin;
	map(_dynamic_vertex_buffer.get(), 0, { 0, 0 }, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_vertex_bytes_written;

	std::vector<st_vk_procedural_vertex> verts;
	verts.reserve(drawcall._positions.size());

	for (uint32_t vert_itr = 0; vert_itr < drawcall._positions.size(); ++vert_itr)
	{
		verts.push_back({ drawcall._positions[vert_itr], drawcall._colors[vert_itr] });
	}

	memcpy(buffer_begin, &verts[0], sizeof(st_vk_procedural_vertex) * verts.size());
	unmap(_dynamic_vertex_buffer.get(), 0, { 0, 0 });

	map(_dynamic_index_buffer.get(), 0, { 0, 0 }, reinterpret_cast<void**>(&buffer_begin));
	buffer_begin += _dynamic_index_bytes_written;

	memcpy(buffer_begin, &drawcall._indices[0], sizeof(uint16_t) * drawcall._indices.size());
	unmap(_dynamic_index_buffer.get(), 0, { 0, 0 });

	st_vk_buffer* dynamic_vertex_buffer = static_cast<st_vk_buffer*>(_dynamic_vertex_buffer.get());
	st_vk_buffer* dynamic_index_buffer = static_cast<st_vk_buffer*>(_dynamic_index_buffer.get());

	vk::DeviceSize offset = vk::DeviceSize(0);
	_command_buffers[st_command_buffer_graphics].bindVertexBuffers(0, 1, &dynamic_vertex_buffer->_buffer, &offset);
	_command_buffers[st_command_buffer_graphics].bindIndexBuffer(dynamic_index_buffer->_buffer, 0, vk::IndexType::eUint16);

	_command_buffers[st_command_buffer_graphics].drawIndexed(
		drawcall._indices.size(),
		1,
		(_dynamic_index_bytes_written / sizeof(uint16_t)),
		(_dynamic_vertex_bytes_written / sizeof(st_vk_procedural_vertex)),
		0);

	_dynamic_vertex_bytes_written += sizeof(st_vk_procedural_vertex) * verts.size();
	_dynamic_index_bytes_written += sizeof(uint16_t) * drawcall._indices.size();
}

void st_vk_graphics_context::dispatch(const st_dispatch_args& args)
{
	_command_buffers[st_command_buffer_graphics].dispatch(args.group_count_x, args.group_count_y, args.group_count_z);
}

void st_vk_graphics_context::create_swap_chain_internal(const st_swap_chain_desc& desc, st_vk_swap_chain* swap_chain)
{
	vk::Win32SurfaceCreateInfoKHR win32_surface_info = vk::Win32SurfaceCreateInfoKHR()
		.setHinstance(GetModuleHandle(NULL))
		.setHwnd((HWND)desc._window_handle);

	VK_VALIDATE(_instance.createWin32SurfaceKHR(&win32_surface_info, nullptr, &swap_chain->_window_surface));

	uint32_t surface_format_count;
	VK_VALIDATE(_gpu.getSurfaceFormatsKHR(swap_chain->_window_surface, &surface_format_count, nullptr));

	std::vector<vk::SurfaceFormatKHR> surface_formats;
	surface_formats.resize(surface_format_count);
	VK_VALIDATE(_gpu.getSurfaceFormatsKHR(swap_chain->_window_surface, &surface_format_count, surface_formats.data()));

	vk::Bool32 surface_support = false;
	VK_VALIDATE(_gpu.getSurfaceSupportKHR(_queue_family_index, swap_chain->_window_surface, &surface_support));
	assert(surface_support);

	vk::SwapchainCreateInfoKHR swap_chain_info = vk::SwapchainCreateInfoKHR()
		.setSurface(swap_chain->_window_surface)
		.setMinImageCount(desc._buffer_count)
		.setImageFormat(convert_format(desc._format))
		.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
		.setImageExtent(vk::Extent2D(desc._width, desc._height))
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setImageSharingMode(vk::SharingMode::eExclusive)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&_queue_family_index)
		.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(vk::PresentModeKHR::eFifo)
		.setClipped(false);

	VK_VALIDATE(_device.createSwapchainKHR(&swap_chain_info, nullptr, &swap_chain->_swap_chain));
	uint32_t backbuffer_count;
	VK_VALIDATE(_device.getSwapchainImagesKHR(swap_chain->_swap_chain, &backbuffer_count, nullptr));
	assert(backbuffer_count = desc._buffer_count);

	vk::Image* buffers = static_cast<vk::Image*>(alloca(sizeof(vk::Image) * backbuffer_count));
	VK_VALIDATE(_device.getSwapchainImagesKHR(swap_chain->_swap_chain, &backbuffer_count, buffers));

	swap_chain->_backbuffers.reserve(backbuffer_count);
	swap_chain->_backbuffer_views.reserve(backbuffer_count);
	for (uint32_t i = 0; i < backbuffer_count; ++i)
	{
		st_vk_texture* buffer = (st_vk_texture*)malloc(sizeof(st_vk_texture));
		memset(buffer, 0, sizeof(st_vk_texture));
		buffer->_device = &_device;
		buffer->_format = desc._format;
		buffer->_width = desc._width;
		buffer->_height = desc._height;
		buffer->_levels = 1;
		buffer->_usage = e_st_texture_usage::color_target;
		buffer->_handle = buffers[i];

		st_texture_view_desc view_desc;
		view_desc._texture = buffer;
		view_desc._format = desc._format;
		view_desc._usage = e_st_view_usage::render_target;

		std::unique_ptr<st_texture_view> view = create_texture_view(view_desc);

		swap_chain->_backbuffers.push_back(buffer);
		swap_chain->_backbuffer_views.push_back(std::move(view));
	}
}

std::unique_ptr<st_swap_chain> st_vk_graphics_context::create_swap_chain(const st_swap_chain_desc& desc)
{
	std::unique_ptr<st_vk_swap_chain> swap_chain = std::make_unique<st_vk_swap_chain>();
	swap_chain->_device = &_device;
	swap_chain->_instance = &_instance;

	create_swap_chain_internal(desc, swap_chain.get());

	return std::move(swap_chain);
}

void st_vk_graphics_context::reconfigure_swap_chain(const st_swap_chain_desc& desc, st_swap_chain* swap_chain_)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);

	for (uint32_t i = 0; i < swap_chain->_backbuffers.size(); ++i)
		free(swap_chain->_backbuffers[i]);
	swap_chain->_backbuffers.clear();
	swap_chain->_backbuffer_views.clear();

	_device.destroySwapchainKHR(swap_chain->_swap_chain, nullptr);
	_instance.destroySurfaceKHR(swap_chain->_window_surface, nullptr);

	create_swap_chain_internal(desc, swap_chain);

	_frame_index = 0;
}

st_texture* st_vk_graphics_context::get_backbuffer(st_swap_chain* swap_chain_, uint32_t index)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);
	return swap_chain->_backbuffers[index];
}

st_texture_view* st_vk_graphics_context::get_backbuffer_view(st_swap_chain* swap_chain_, uint32_t index)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);
	return swap_chain->_backbuffer_views[index].get();
}

e_st_swap_chain_status st_vk_graphics_context::acquire_backbuffer(st_swap_chain* swap_chain_)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);

	e_st_swap_chain_status status = e_st_swap_chain_status::current;

	uint32_t backbuffer_index;
	vk::Result result = _device.acquireNextImageKHR(
		swap_chain->_swap_chain,
		std::numeric_limits<uint64_t>::max(),
		vk::Semaphore(nullptr),
		_acquire_fence,
		&backbuffer_index);
	// TODO: Anything special with suboptimalKHR here?
	if (result == vk::Result::eErrorOutOfDateKHR)
		status = e_st_swap_chain_status::out_of_date;

	assert(backbuffer_index == _frame_index);

	VK_VALIDATE(_device.waitForFences(1, &_acquire_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_acquire_fence));

	return status;
}

void st_vk_graphics_context::begin_loading()
{
	vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	VK_VALIDATE(_command_buffers[st_command_buffer_loading].begin(&begin_info));
}

void st_vk_graphics_context::end_loading()
{
	VK_VALIDATE(_command_buffers[st_command_buffer_loading].end());

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(1)
		.setPCommandBuffers(_command_buffers);

	vk::Result result = _device.getFenceStatus(_fence);
	VK_VALIDATE(_queue.submit(1, &submit_info, _fence));

	VK_VALIDATE(_device.waitForFences(1, &_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_fence));
}

void st_vk_graphics_context::begin_frame()
{
	vk::CommandBufferBeginInfo begin_info = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	VK_VALIDATE(_command_buffers[st_command_buffer_loading].begin(&begin_info));
	VK_VALIDATE(_command_buffers[st_command_buffer_graphics].begin(&begin_info));

	// Begin writing to the head of the upload buffer again.
	_upload_buffer_offset = 0;

	_dynamic_vertex_bytes_written = 0;
	_dynamic_index_bytes_written = 0;

	// Reset the descriptor set pool.
	if (_descriptor_set_pool[_frame_index].size() > 0)
	{
		VK_VALIDATE(_device.freeDescriptorSets(
			_descriptor_pool,
			_descriptor_set_pool[_frame_index].size(),
			_descriptor_set_pool[_frame_index].data()));
		_descriptor_set_pool[_frame_index].clear();
	}
}

void st_vk_graphics_context::end_frame()
{
	if (_upload_buffer_offset > 0)
	{
		size_t flush_size = align_value(_upload_buffer_offset, static_cast<uint32_t>(_caps.limits.nonCoherentAtomSize));

		// This flush may have a performance cost.
		vk::MappedMemoryRange mapped_range = vk::MappedMemoryRange()
			.setMemory(_upload_buffer_memory)
			.setOffset(vk::DeviceSize(0))
			.setSize(vk::DeviceSize(flush_size));
		VK_VALIDATE(_device.flushMappedMemoryRanges(1, &mapped_range));
	}
}

void st_vk_graphics_context::execute()
{
	VK_VALIDATE(_command_buffers[st_command_buffer_loading].end());
	VK_VALIDATE(_command_buffers[st_command_buffer_graphics].end());

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(2)
		.setPCommandBuffers(_command_buffers);

	vk::Result result = _device.getFenceStatus(_fence);
	VK_VALIDATE(_queue.submit(1, &submit_info, _fence));

	// TODO: Better parallelization.
	VK_VALIDATE(_device.waitForFences(1, &_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_fence));
}

void st_vk_graphics_context::present(st_swap_chain* swap_chain_)
{
	st_vk_swap_chain* swap_chain = static_cast<st_vk_swap_chain*>(swap_chain_);

	VK_VALIDATE(_command_buffers[st_command_buffer_loading].end());
	VK_VALIDATE(_command_buffers[st_command_buffer_graphics].end());

	vk::SubmitInfo submit_info = vk::SubmitInfo()
		.setCommandBufferCount(2)
		.setPCommandBuffers(_command_buffers);

	vk::Result result = _device.getFenceStatus(_fence);
	VK_VALIDATE(_queue.submit(1, &submit_info, _fence));

	vk::PresentInfoKHR present_info = vk::PresentInfoKHR()
		.setSwapchainCount(1)
		.setPSwapchains(&swap_chain->_swap_chain)
		.setPImageIndices(&_frame_index);

	result = _queue.presentKHR(&present_info);
	// TODO: Anything special with suboptimalKHR here?

	// TODO: Better parallelization.
	VK_VALIDATE(_device.waitForFences(1, &_fence, true, std::numeric_limits<uint64_t>::max()));
	VK_VALIDATE(_device.resetFences(1, &_fence));

	_frame_index = (_frame_index + 1) % k_max_frames;
}

void st_vk_graphics_context::wait_for_idle()
{
	VK_VALIDATE(_device.waitIdle());
}

void st_vk_graphics_context::begin_marker(const std::string& marker)
{
	if (_has_markers)
	{
		vk::DebugMarkerMarkerInfoEXT marker_info = vk::DebugMarkerMarkerInfoEXT()
			.setPMarkerName(marker.c_str());

		_command_buffers[st_command_buffer_graphics].debugMarkerBeginEXT(&marker_info);
	}
}

void st_vk_graphics_context::end_marker()
{
	if (_has_markers)
	{
		_command_buffers[st_command_buffer_graphics].debugMarkerEndEXT();
	}
}

std::unique_ptr<st_texture> st_vk_graphics_context::create_texture(const st_texture_desc& desc)
{
	std::unique_ptr<st_vk_texture> texture = std::make_unique<st_vk_texture>();
	texture->_device = &_device;
	texture->_width = desc._width;
	texture->_height = desc._height;
	texture->_levels = desc._levels;
	texture->_format = desc._format;
	texture->_usage = desc._usage;

	vk::ImageUsageFlags flags;

	if (desc._usage & e_st_texture_usage::copy_source) flags |= vk::ImageUsageFlagBits::eTransferSrc;
	if (desc._usage & e_st_texture_usage::copy_dest) flags |= vk::ImageUsageFlagBits::eTransferDst;
	if (desc._usage & e_st_texture_usage::sampled) flags |= vk::ImageUsageFlagBits::eSampled;
	if (desc._usage & e_st_texture_usage::storage) flags |= vk::ImageUsageFlagBits::eStorage;
	if (desc._usage & e_st_texture_usage::color_target) flags |= vk::ImageUsageFlagBits::eColorAttachment;
	if (desc._usage & e_st_texture_usage::depth_target) flags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
	if (desc._usage & e_st_texture_usage::transient_target) flags |= vk::ImageUsageFlagBits::eTransientAttachment;
	if (desc._usage & e_st_texture_usage::input_target) flags |= vk::ImageUsageFlagBits::eInputAttachment;

	// TODO: For now, create all with eTransferDst. This is the correct usage for the
	// buffer copy to upload to the image.
	flags |= vk::ImageUsageFlagBits::eTransferDst;

	vk::ImageCreateInfo create_info = vk::ImageCreateInfo()
		.setArrayLayers(1)
		.setImageType(vk::ImageType::e2D)
		.setFormat(convert_format(desc._format))
		.setExtent(vk::Extent3D(desc._width, desc._height, 1))
		.setMipLevels(desc._levels)
		.setUsage(flags)
		.setInitialLayout(vk::ImageLayout::eUndefined)
		.setTiling(vk::ImageTiling::eOptimal);

	VK_VALIDATE(_device.createImage(&create_info, nullptr, &texture->_handle));

	// Allocate memory for the image.
	vk::MemoryRequirements memory_reqs;
	_device.getImageMemoryRequirements(texture->_handle, &memory_reqs);

	vk::MemoryAllocateInfo allocate_info = vk::MemoryAllocateInfo()
		.setAllocationSize(memory_reqs.size)
		.setMemoryTypeIndex(_device_memory_index);

	VK_VALIDATE(_device.allocateMemory(&allocate_info, nullptr, &texture->_memory));
	VK_VALIDATE(_device.bindImageMemory(texture->_handle, texture->_memory, 0));

	// Transition the image to its intended state.
	vk::ImageLayout dst_layout = (desc._data != nullptr) ? vk::ImageLayout::eTransferDstOptimal : convert_resource_state(desc._initial_state);
	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (desc._format == st_format_d16_unorm ||
		desc._format == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (desc._format == st_format_d24_unorm_s8_uint)
	{
		aspect = vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil;
	}

	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(desc._levels);
	vk::ImageMemoryBarrier barriers[] =
	{
		vk::ImageMemoryBarrier()
		.setImage(texture->_handle)
		.setOldLayout(vk::ImageLayout::eUndefined)
		.setNewLayout(dst_layout)
		.setSubresourceRange(range),
	};

	_command_buffers[st_command_buffer_loading].pipelineBarrier(
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eTransfer,
		vk::DependencyFlags(),
		0,
		nullptr,
		0,
		nullptr,
		1,
		barriers);

	if (desc._data)
	{
		std::vector<vk::BufferImageCopy> regions;
		uint64_t offset = 0;
		for (int i = 0; i < desc._levels; ++i)
		{
			uint32_t level_width = desc._width >> i;
			uint32_t level_height = desc._height >> i;

			// Stop on mips less than size 4.
			// TODO: This should be mips less than the texel block size of the compressed format.
			if (is_compressed(desc._format) &&
				(level_width < 4 || level_height < 4))
				break;

			size_t bpp = bits_per_pixel(desc._format);
			size_t num_bytes;
			get_surface_info(
				level_width,
				level_height,
				desc._format,
				&num_bytes,
				nullptr,
				nullptr);

			// Align the upload buffer offset to the pixel size to satisfy Vulkan requirement.
			_upload_buffer_offset = align_value(_upload_buffer_offset, bpp);

			memcpy(
				reinterpret_cast<uint8_t*>(_upload_buffer_head) + _upload_buffer_offset,
				reinterpret_cast<uint8_t*>(desc._data) + offset,
				num_bytes);

			vk::ImageSubresourceLayers subresource = vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				.setMipLevel(i);

			vk::BufferImageCopy region = vk::BufferImageCopy()
				.setBufferImageHeight(level_height)
				.setBufferRowLength(level_width)
				.setBufferOffset(_upload_buffer_offset)
				.setImageExtent(vk::Extent3D(level_width, level_height, 1))
				.setImageOffset(vk::Offset3D(0, 0, 0))
				.setImageSubresource(subresource);

			regions.push_back(region);

			offset += num_bytes;
			_upload_buffer_offset += num_bytes;
		}

		_command_buffers[st_command_buffer_loading].copyBufferToImage(
			_upload_buffer,
			texture->_handle,
			vk::ImageLayout::eTransferDstOptimal,
			regions.size(),
			regions.data());

		vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
			.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setLayerCount(1)
			.setBaseMipLevel(0)
			.setLevelCount(desc._levels);

		vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
			.setImage(texture->_handle)
			.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
			.setNewLayout(convert_resource_state(desc._initial_state))
			.setSubresourceRange(range);
		_command_buffers[st_command_buffer_loading].pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eTransfer,
			vk::DependencyFlags(),
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);
	}

	texture->_state = desc._initial_state;

	return std::move(texture);
}

void st_vk_graphics_context::set_texture_name(st_texture* texture_, std::string name)
{
	st_vk_texture* texture = static_cast<st_vk_texture*>(texture_);

	vk::DebugUtilsObjectNameInfoEXT name_info = vk::DebugUtilsObjectNameInfoEXT()
		.setObjectType(vk::ObjectType::eImage)
		.setObjectHandle(VK_GET_HANDLE(texture->_handle, VkImage))
		.setPObjectName(name.c_str());

	VK_VALIDATE(_device.setDebugUtilsObjectNameEXT(&name_info));
}

void st_vk_graphics_context::transition(
	st_texture* texture_,
	e_st_texture_state new_state)
{
	st_vk_texture* texture = static_cast<st_vk_texture*>(texture_);
	e_st_texture_state old_state = texture->_state;

	if (old_state == new_state)
		return;

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (texture->_format == st_format_d16_unorm ||
		texture->_format == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (texture->_format == st_format_d24_unorm_s8_uint)
	{
		aspect = vk::ImageAspectFlagBits::eDepth |
			vk::ImageAspectFlagBits::eStencil;
	}

	// Transition the present target to the optimal layout for copy.
	vk::ImageSubresourceRange range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseArrayLayer(0)
		.setLayerCount(1)
		.setBaseMipLevel(0)
		.setLevelCount(texture->_levels);

	vk::ImageMemoryBarrier barriers[] =
	{
		vk::ImageMemoryBarrier()
		.setImage(texture->_handle)
		.setOldLayout(convert_resource_state(old_state))
		.setNewLayout(convert_resource_state(new_state))
		.setSubresourceRange(range),
	};

	texture->_state = new_state;

	_command_buffers[st_command_buffer_graphics].pipelineBarrier(
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

std::unique_ptr<st_texture_view> st_vk_graphics_context::create_texture_view(const st_texture_view_desc& desc)
{
	const st_vk_texture* texture = static_cast<const st_vk_texture*>(desc._texture);

	std::unique_ptr<st_vk_texture_view> view = std::make_unique<st_vk_texture_view>();
	view->_device = &_device;

	vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor;

	if (texture->_format == st_format_d16_unorm ||
		texture->_format == st_format_d32_float)
	{
		aspect = vk::ImageAspectFlagBits::eDepth;
	}
	else if (texture->_format == st_format_d24_unorm_s8_uint)
	{
		// TODO: Can only create one image view per aspect, per Vulkan spec.
		aspect = vk::ImageAspectFlagBits::eDepth;
			//| vk::ImageAspectFlagBits::eStencil;
	}

	vk::ImageSubresourceRange subresource_range = vk::ImageSubresourceRange()
		.setAspectMask(aspect)
		.setBaseMipLevel(desc._first_mip)
		.setLevelCount(desc._mips)
		.setBaseArrayLayer(desc._first_slice)
		.setLayerCount(desc._slices);

	vk::ImageViewCreateInfo create_info = vk::ImageViewCreateInfo()
		.setFormat(convert_format(desc._format))
		.setImage(texture->_handle)
		.setViewType(vk::ImageViewType::e2D)
		.setSubresourceRange(subresource_range);

	VK_VALIDATE(_device.createImageView(&create_info, nullptr, &view->_view));

	return std::move(view);
}

std::unique_ptr<st_sampler> st_vk_graphics_context::create_sampler(const st_sampler_desc& desc)
{
	std::unique_ptr<st_vk_sampler> sampler = std::make_unique<st_vk_sampler>();
	sampler->_device = &_device;

	vk::SamplerCreateInfo create_info = vk::SamplerCreateInfo()
		.setMagFilter(convert_filter(desc._mag_filter))
		.setMinFilter(convert_filter(desc._min_filter))
		.setMipmapMode(desc._mip_filter == st_filter_nearest ? vk::SamplerMipmapMode::eNearest : vk::SamplerMipmapMode::eLinear)
		.setAddressModeU(convert_address_mode(desc._address_u))
		.setAddressModeV(convert_address_mode(desc._address_v))
		.setAddressModeW(convert_address_mode(desc._address_w))
		.setMipLodBias(desc._mip_bias)
		.setAnisotropyEnable(false)
		.setCompareEnable(desc._compare_func != st_compare_func_never)
		.setCompareOp(convert_compare_op(desc._compare_func))
		.setMinLod(desc._min_mip)
		.setMaxLod(desc._max_mip)
		.setUnnormalizedCoordinates(false);

	VK_VALIDATE(_device.createSampler(&create_info, nullptr, &sampler->_sampler));

	return std::move(sampler);
}

std::unique_ptr<st_buffer> st_vk_graphics_context::create_buffer(const st_buffer_desc& desc)
{
	std::unique_ptr<st_vk_buffer> buffer = std::make_unique<st_vk_buffer>();
	buffer->_device = &_device;
	buffer->_count = desc._count;
	buffer->_element_size = desc._element_size;
	buffer->_usage = desc._usage;

	vk::BufferUsageFlags flags;

	if (desc._usage & e_st_buffer_usage::index) flags |= vk::BufferUsageFlagBits::eIndexBuffer;
	if (desc._usage & e_st_buffer_usage::indirect) flags |= vk::BufferUsageFlagBits::eIndirectBuffer;
	if (desc._usage & e_st_buffer_usage::storage) flags |= vk::BufferUsageFlagBits::eStorageBuffer;
	if (desc._usage & e_st_buffer_usage::storage_texel) flags |= vk::BufferUsageFlagBits::eStorageTexelBuffer;
	if (desc._usage & e_st_buffer_usage::transfer_dest) flags |= vk::BufferUsageFlagBits::eTransferDst;
	if (desc._usage & e_st_buffer_usage::transfer_source) flags |= vk::BufferUsageFlagBits::eTransferSrc;
	if (desc._usage & e_st_buffer_usage::uniform) flags |= vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst;
	if (desc._usage & e_st_buffer_usage::uniform_texel) flags |= vk::BufferUsageFlagBits::eUniformTexelBuffer;
	if (desc._usage & e_st_buffer_usage::vertex) flags |= vk::BufferUsageFlagBits::eVertexBuffer;

	vk::BufferCreateInfo buffer_info = vk::BufferCreateInfo()
		.setUsage(flags)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&_queue_family_index)
		.setSize(align_value(desc._count * desc._element_size, 4));

	VK_VALIDATE(_device.createBuffer(&buffer_info, nullptr, &buffer->_buffer));

	// Allocate memory for the buffer.
	vk::MemoryRequirements memory_reqs;
	_device.getBufferMemoryRequirements(buffer->_buffer, &memory_reqs);

	uint32_t memory_index = _device_memory_index;
	if (desc._usage & e_st_buffer_usage::transfer_dest) memory_index = _mapped_memory_index;

	vk::MemoryAllocateInfo allocate_info = vk::MemoryAllocateInfo()
		.setAllocationSize(memory_reqs.size)
		.setMemoryTypeIndex(memory_index);

	VK_VALIDATE(_device.allocateMemory(&allocate_info, nullptr, &buffer->_memory));

	VK_VALIDATE(_device.bindBufferMemory(buffer->_buffer, buffer->_memory, 0));

	return std::move(buffer);
}

std::unique_ptr<st_buffer_view> st_vk_graphics_context::create_buffer_view(const st_buffer_view_desc& desc)
{
	std::unique_ptr<st_vk_buffer_view> view = std::make_unique<st_vk_buffer_view>();
	view->_device = &_device;

	const st_vk_buffer* buffer = static_cast<const st_vk_buffer*>(desc._buffer);
	view->_buffer = buffer;
	view->_first_element = desc._first_element;
	view->_element_count = desc._element_count;

	// In the Vulkan spec, vkBufferView objects are only allowed for buffers created with
	// the uniform texel buffer bit or texel storage buffer bit, so skip this if that's
	// not the case. When setting resources in descriptor sets, the buffer view isn't used
	// anyway.
	if (buffer->_usage & e_st_buffer_usage::uniform_texel ||
		buffer->_usage & e_st_buffer_usage::storage_texel)
	{
		vk::DeviceSize range = (desc._element_count == k_whole_size) ?
			vk::DeviceSize(VK_WHOLE_SIZE) :
			desc._element_count * buffer->_element_size;

		vk::BufferViewCreateInfo create_info = vk::BufferViewCreateInfo()
			.setBuffer(buffer->_buffer)
			.setOffset(desc._first_element * buffer->_element_size)
			.setRange(range);

		VK_VALIDATE(_device.createBufferView(&create_info, nullptr, &view->_view));
	}

	return std::move(view);
}

void st_vk_graphics_context::map(st_buffer* buffer_, uint32_t subresource, const st_range& range, void** outData)
{
	st_vk_buffer* buffer = static_cast<st_vk_buffer*>(buffer_);

	uint64_t size = (range.end - range.begin);
	if (range.end == 0)
		size = vk::WholeSize;

	VK_VALIDATE(_device.mapMemory(buffer->_memory, range.begin, size, vk::MemoryMapFlags(0), outData));
}

void st_vk_graphics_context::unmap(st_buffer* buffer_, uint32_t subresource, const st_range& range)
{
	st_vk_buffer* buffer = static_cast<st_vk_buffer*>(buffer_);

	_device.unmapMemory(buffer->_memory);
}

void st_vk_graphics_context::update_buffer(st_buffer* buffer_, void* data, const uint32_t offset, const uint32_t count)
{
	st_vk_buffer* buffer = static_cast<st_vk_buffer*>(buffer_);

	_command_buffers[st_command_buffer_loading].updateBuffer(
		buffer->_buffer,
		offset,
		align_value(count * buffer->_element_size, 4),
		data);
}

void st_vk_graphics_context::set_buffer_name(st_buffer* buffer_, std::string name)
{
	st_vk_buffer* buffer = static_cast<st_vk_buffer*>(buffer_);

	vk::DebugUtilsObjectNameInfoEXT name_info = vk::DebugUtilsObjectNameInfoEXT()
		.setObjectType(vk::ObjectType::eBuffer)
		.setObjectHandle(VK_GET_HANDLE(buffer->_buffer, VkBuffer))
		.setPObjectName(name.c_str());

	VK_VALIDATE(_device.setDebugUtilsObjectNameEXT(&name_info));
}

std::unique_ptr<st_resource_table> st_vk_graphics_context::create_resource_table()
{
	std::unique_ptr<st_vk_resource_table> table = std::make_unique<st_vk_resource_table>();
	table->_device = &_device;
	table->_pool = &_descriptor_pool;

	auto create_set = [this](e_st_descriptor_slot slot, vk::DescriptorSet* set)
	{
		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(&_graphics_layouts[slot]);

		VK_VALIDATE(_device.allocateDescriptorSets(&allocate_info, set));
	};

	create_set(st_descriptor_slot_textures, &table->_textures);
	create_set(st_descriptor_slot_samplers, &table->_samplers);
	create_set(st_descriptor_slot_constants, &table->_constants);
	create_set(st_descriptor_slot_buffers, &table->_buffers);

	return std::move(table);
}

std::unique_ptr<st_resource_table> st_vk_graphics_context::create_resource_table_compute()
{
	std::unique_ptr<st_vk_resource_table> table = std::make_unique<st_vk_resource_table>();
	table->_device = &_device;
	table->_pool = &_descriptor_pool;

	auto create_set = [this](e_st_descriptor_slot slot, vk::DescriptorSet* set)
	{
		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(&_compute_layouts[slot]);

		VK_VALIDATE(_device.allocateDescriptorSets(&allocate_info, set));
	};

	create_set(st_descriptor_slot_textures, &table->_textures);
	create_set(st_descriptor_slot_samplers, &table->_samplers);
	create_set(st_descriptor_slot_constants, &table->_constants);
	create_set(st_descriptor_slot_buffers, &table->_uavs);

	return std::move(table);
}

void st_vk_graphics_context::set_constant_buffers(st_resource_table* table_, uint32_t count, const st_buffer_view** cbs)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);
	table->_constant_count = count;

	std::vector<vk::DescriptorBufferInfo> infos;
	for (int i = 0; i < count; ++i)
	{
		const st_vk_buffer_view* cb = static_cast<const st_vk_buffer_view*>(cbs[i]);

		vk::DeviceSize range = (cb->_element_count == k_whole_size) ?
			vk::DeviceSize(VK_WHOLE_SIZE) :
			cb->_element_count * cb->_buffer->_element_size;

		infos.emplace_back() = vk::DescriptorBufferInfo()
			.setBuffer(cb->_buffer->_buffer)
			.setOffset(cb->_first_element * cb->_buffer->_element_size)
			.setRange(range);
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setDescriptorCount(count)
		.setDstSet(table->_constants)
		.setDstBinding(0)
		.setPBufferInfo(infos.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_graphics_context::set_textures(
	st_resource_table* table_,
	uint32_t count,
	const st_texture_view** textures,
	const st_sampler** samplers)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);
	table->_texture_count = count;
	table->_sampler_count = count;

	table->_sampler_resources.reserve(count);

	std::vector<vk::DescriptorImageInfo> images;
	for (int i = 0; i < count; ++i)
	{
		vk::ImageView view = vk::ImageView();

		if (textures)
		{
			const st_vk_texture_view* texture = static_cast<const st_vk_texture_view*>(textures[i]);
			view = texture->_view;
		}

		const st_vk_sampler* sampler = static_cast<const st_vk_sampler*>(samplers[i]);
		table->_sampler_resources.push_back(sampler);

		images.emplace_back() = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(view)
			.setSampler(sampler->_sampler);
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(count)
		.setDstSet(table->_textures)
		.setDstBinding(0)
		.setPImageInfo(images.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);

	write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampler)
		.setDescriptorCount(count)
		.setDstSet(table->_samplers)
		.setDstBinding(0)
		.setPImageInfo(images.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_graphics_context::set_buffers(st_resource_table* table_, uint32_t count, const st_buffer_view** buffers)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);
	table->_buffer_count = count;

	std::vector<vk::DescriptorBufferInfo> infos;
	for (int i = 0; i < count; ++i)
	{
		const st_vk_buffer_view* bv = static_cast<const st_vk_buffer_view*>(buffers[i]);

		vk::DeviceSize range = (bv->_element_count == k_whole_size) ?
			vk::DeviceSize(VK_WHOLE_SIZE) :
			bv->_element_count * bv->_buffer->_element_size;

		infos.emplace_back() = vk::DescriptorBufferInfo()
			.setBuffer(bv->_buffer->_buffer)
			.setOffset(bv->_first_element * bv->_buffer->_element_size)
			.setRange(range);
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eStorageBuffer)
		.setDescriptorCount(count)
		.setDstSet(table->_buffers)
		.setDstBinding(0)
		.setPBufferInfo(infos.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_graphics_context::set_uavs(st_resource_table* table_, uint32_t count, const st_texture_view** textures)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);
	table->_uav_count = count;

	std::vector<vk::DescriptorImageInfo> infos;
	for (int i = 0; i < count; ++i)
	{
		const st_vk_texture_view* texture = static_cast<const st_vk_texture_view*>(textures[i]);
		vk::ImageView view = texture->_view;

		infos.emplace_back() = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eGeneral)
			.setImageView(view);
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eStorageImage)
		.setDescriptorCount(count)
		.setDstSet(table->_uavs)
		.setDstBinding(0)
		.setPImageInfo(infos.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_graphics_context::update_textures(st_resource_table* table_, uint32_t count, const st_texture_view** views)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);

	std::vector<vk::DescriptorImageInfo> images;
	for (uint32_t itr = 0; itr < count; ++itr)
	{
		const st_vk_texture_view* view = static_cast<const st_vk_texture_view*>(views[itr]);

		images.emplace_back() = vk::DescriptorImageInfo()
			.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(view->_view)
			.setSampler(table->_sampler_resources[itr]->_sampler);
	}

	vk::WriteDescriptorSet write_set = vk::WriteDescriptorSet()
		.setDescriptorType(vk::DescriptorType::eSampledImage)
		.setDescriptorCount(count)
		.setDstSet(table->_textures)
		.setDstBinding(0)
		.setPImageInfo(images.data());

	_device.updateDescriptorSets(1, &write_set, 0, nullptr);
}

void st_vk_graphics_context::bind_resources(st_resource_table* table_)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);

	auto bind_set = [this](e_st_descriptor_slot slot, const vk::DescriptorSet* set, uint32_t count)
	{
		// First, copy the descriptor set to a unique one for this frame.
		vk::DescriptorSet& new_set = _descriptor_set_pool[_frame_index].emplace_back();

		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(&_graphics_layouts[slot]);

		VK_VALIDATE(_device.allocateDescriptorSets(&allocate_info, &new_set));

		vk::CopyDescriptorSet copy_set = vk::CopyDescriptorSet()
			.setDescriptorCount(count)
			.setSrcSet(*set)
			.setSrcBinding(0)
			.setDstSet(new_set)
			.setDstBinding(0);

		_device.updateDescriptorSets(0, nullptr, 1, &copy_set);

		_command_buffers[st_command_buffer_graphics].bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			_graphics_signature,
			slot,
			1,
			&new_set,
			0,
			nullptr);
	};

	bind_set(st_descriptor_slot_textures, &table->_textures, table->_texture_count);
	bind_set(st_descriptor_slot_samplers, &table->_samplers, table->_sampler_count);
	bind_set(st_descriptor_slot_constants, &table->_constants, table->_constant_count);
	bind_set(st_descriptor_slot_buffers, &table->_buffers, table->_buffer_count);
}

void st_vk_graphics_context::bind_compute_resources(st_resource_table* table_)
{
	st_vk_resource_table* table = static_cast<st_vk_resource_table*>(table_);

	auto bind_set = [this](e_st_descriptor_slot slot, const vk::DescriptorSet* set, uint32_t count)
	{
		// First, copy the descriptor set to a unique one for this frame.
		vk::DescriptorSet& new_set = _descriptor_set_pool[_frame_index].emplace_back();

		vk::DescriptorSetAllocateInfo allocate_info = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(_descriptor_pool)
			.setDescriptorSetCount(1)
			.setPSetLayouts(&_compute_layouts[slot]);

		VK_VALIDATE(_device.allocateDescriptorSets(&allocate_info, &new_set));

		vk::CopyDescriptorSet copy_set = vk::CopyDescriptorSet()
			.setDescriptorCount(count)
			.setSrcSet(*set)
			.setSrcBinding(0)
			.setDstSet(new_set)
			.setDstBinding(0);

		_device.updateDescriptorSets(0, nullptr, 1, &copy_set);

		_command_buffers[st_command_buffer_graphics].bindDescriptorSets(
			vk::PipelineBindPoint::eCompute,
			_compute_signature,
			slot,
			1,
			&new_set,
			0,
			nullptr);
	};

	bind_set(st_descriptor_slot_textures, &table->_textures, table->_texture_count);
	bind_set(st_descriptor_slot_samplers, &table->_samplers, table->_sampler_count);
	bind_set(st_descriptor_slot_constants, &table->_constants, table->_constant_count);
	bind_set(st_descriptor_slot_uavs, &table->_uavs, table->_uav_count);
}

std::unique_ptr<st_shader> st_vk_graphics_context::create_shader(const char* filename, e_st_shader_type_flags type)
{
	std::unique_ptr<st_vk_shader> shader = std::make_unique<st_vk_shader>();
	shader->_device = &_device;
	shader->_type = uint8_t(type);

	auto load_shader = [this](std::string file_name, vk::ShaderModule& shader)
	{
		std::string full_path = std::string(g_root_path) + std::string(file_name);

		std::ifstream shader_file(full_path, std::ios::binary);
		std::vector<unsigned char> shader_blob(std::istreambuf_iterator<char>(shader_file), {});

		vk::ShaderModuleCreateInfo create_info = vk::ShaderModuleCreateInfo()
			.setCodeSize(shader_blob.size())
			.setPCode(reinterpret_cast<uint32_t*>(shader_blob.data()));

		VK_VALIDATE(_device.createShaderModule(&create_info, nullptr, &shader));
	};

	if (type & e_st_shader_type::vertex)
	{
		load_shader(
			std::string(filename) + std::string("_vert.spirv"),
			shader->_vs);
	}

	if (type & e_st_shader_type::pixel)
	{
		load_shader(
			std::string(filename) + std::string("_frag.spirv"),
			shader->_ps);
	}

	if (type & e_st_shader_type::compute)
	{
		load_shader(
			std::string(filename) + std::string("_comp.spirv"),
			shader->_cs);
	}

	return std::move(shader);
}

std::unique_ptr<st_pipeline> st_vk_graphics_context::create_graphics_pipeline(const st_graphics_state_desc& desc)
{
	std::unique_ptr<st_vk_pipeline> pipeline = std::make_unique<st_vk_pipeline>();
	pipeline->_device = &_device;

	const st_vk_shader* shader = static_cast<const st_vk_shader*>(desc._shader);

	std::vector<vk::PipelineShaderStageCreateInfo> stages;

	if (e_st_shader_type_flags(shader->_type) & e_st_shader_type::vertex)
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(shader->_vs)
			.setPName("vs_main");
	}
	if (e_st_shader_type_flags(shader->_type) & e_st_shader_type::pixel)
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(shader->_ps)
			.setPName("ps_main");
	}
	if (e_st_shader_type_flags(shader->_type) & e_st_shader_type::domain)
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eTessellationEvaluation)
			.setModule(shader->_ds)
			.setPName("ds_main");
	}
	if (e_st_shader_type_flags(shader->_type) & e_st_shader_type::hull)
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eTessellationControl)
			.setModule(shader->_hs)
			.setPName("hs_main");
	}
	if (e_st_shader_type_flags(shader->_type) & e_st_shader_type::geometry)
	{
		stages.emplace_back()
			.setStage(vk::ShaderStageFlagBits::eGeometry)
			.setModule(shader->_gs)
			.setPName("gs_main");
	}

	vk::PipelineInputAssemblyStateCreateInfo input_assembly = vk::PipelineInputAssemblyStateCreateInfo()
		.setTopology(convert_topology_type(desc._primitive_topology_type));

	const st_vk_render_pass* render_pass = static_cast<const st_vk_render_pass*>(desc._pass);

	vk::Rect2D scissor = vk::Rect2D()
		.setOffset(vk::Offset2D())
		.setExtent(vk::Extent2D(uint32_t(render_pass->_viewport.width), uint32_t(-render_pass->_viewport.height)));

	vk::PipelineViewportStateCreateInfo viewport = vk::PipelineViewportStateCreateInfo()
		.setViewportCount(1)
		.setPViewports(&render_pass->_viewport)
		.setScissorCount(1)
		.setPScissors(&scissor);

	vk::PipelineRasterizationStateCreateInfo raster = vk::PipelineRasterizationStateCreateInfo()
		.setCullMode(convert_cull_mode(desc._rasterizer_desc._cull_mode))
		.setPolygonMode(convert_fill_mode(desc._rasterizer_desc._fill_mode))
		.setFrontFace(vk::FrontFace(desc._rasterizer_desc._winding_order_clockwise))
		.setDepthClampEnable(false)
		.setRasterizerDiscardEnable(false)
		.setDepthBiasEnable(false)
		.setLineWidth(1.0f);

	vk::PipelineMultisampleStateCreateInfo multisample = vk::PipelineMultisampleStateCreateInfo()
		.setAlphaToCoverageEnable(desc._blend_desc._alpha_to_coverage)
		.setAlphaToOneEnable(false)
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setSampleShadingEnable(false);

	vk::StencilOpState stencil_front = vk::StencilOpState()
		.setFailOp(convert_stencil_op(desc._depth_stencil_desc._front_stencil._stencil_fail_op))
		.setDepthFailOp(convert_stencil_op(desc._depth_stencil_desc._front_stencil._depth_fail_op))
		.setPassOp(convert_stencil_op(desc._depth_stencil_desc._front_stencil._stencil_pass_op))
		.setCompareOp(convert_compare_op(desc._depth_stencil_desc._front_stencil._stencil_func))
		.setCompareMask(desc._depth_stencil_desc._stencil_read_mask)
		.setWriteMask(desc._depth_stencil_desc._stencil_write_mask)
		.setReference(desc._depth_stencil_desc._stencil_ref);

	vk::StencilOpState stencil_back = vk::StencilOpState()
		.setFailOp(convert_stencil_op(desc._depth_stencil_desc._back_stencil._stencil_fail_op))
		.setDepthFailOp(convert_stencil_op(desc._depth_stencil_desc._back_stencil._depth_fail_op))
		.setPassOp(convert_stencil_op(desc._depth_stencil_desc._back_stencil._stencil_pass_op))
		.setCompareOp(convert_compare_op(desc._depth_stencil_desc._back_stencil._stencil_func))
		.setCompareMask(desc._depth_stencil_desc._stencil_read_mask)
		.setWriteMask(desc._depth_stencil_desc._stencil_write_mask)
		.setReference(desc._depth_stencil_desc._stencil_ref);

	vk::PipelineDepthStencilStateCreateInfo depth_stencil = vk::PipelineDepthStencilStateCreateInfo()
		.setDepthTestEnable(desc._depth_stencil_desc._depth_enable)
		.setDepthWriteEnable(desc._depth_stencil_desc._depth_mask)
		.setDepthCompareOp(convert_compare_op(desc._depth_stencil_desc._depth_compare))
		.setDepthBoundsTestEnable(false)
		.setStencilTestEnable(desc._depth_stencil_desc._stencil_enable)
		.setFront(stencil_front)
		.setBack(stencil_back);

	std::vector<vk::PipelineColorBlendAttachmentState> attachment_states;
	for (int i = 0; i < desc._render_target_count; ++i)
	{
		vk::PipelineColorBlendAttachmentState state = vk::PipelineColorBlendAttachmentState()
			.setBlendEnable(desc._blend_desc._target_blend[i]._blend)
			.setColorBlendOp(convert_blend_op(desc._blend_desc._target_blend[i]._blend_op))
			.setSrcColorBlendFactor(convert_blend(desc._blend_desc._target_blend[i]._src_blend))
			.setDstColorBlendFactor(convert_blend(desc._blend_desc._target_blend[i]._dst_blend))
			.setAlphaBlendOp(convert_blend_op(desc._blend_desc._target_blend[i]._blend_op_alpha))
			.setSrcAlphaBlendFactor(convert_blend(desc._blend_desc._target_blend[i]._src_blend_alpha))
			.setDstAlphaBlendFactor(convert_blend(desc._blend_desc._target_blend[i]._dst_blend_alpha))
			.setColorWriteMask(vk::ColorComponentFlags(desc._blend_desc._target_blend[i]._write_mask));

		attachment_states.push_back(state);
	}

	vk::PipelineColorBlendStateCreateInfo color_blend = vk::PipelineColorBlendStateCreateInfo()
		.setAttachmentCount(desc._render_target_count)
		.setPAttachments(attachment_states.data())
		.setLogicOpEnable(false);

	std::vector<vk::DynamicState> dynamic_states;
	dynamic_states.push_back(vk::DynamicState::eViewportWithCount);
	dynamic_states.push_back(vk::DynamicState::eScissorWithCount);

	vk::PipelineDynamicStateCreateInfo dynamic_state = vk::PipelineDynamicStateCreateInfo()
		.setDynamicStateCount(dynamic_states.size())
		.setPDynamicStates(dynamic_states.data());

	const st_vk_vertex_format* vertex_format = static_cast<const st_vk_vertex_format*>(desc._vertex_format);

	vk::GraphicsPipelineCreateInfo create_info = vk::GraphicsPipelineCreateInfo()
		.setStageCount(stages.size())
		.setPStages(stages.data())
		.setPVertexInputState(&vertex_format->_input_layout)
		.setPInputAssemblyState(&input_assembly)
		.setPViewportState(nullptr)
		.setRenderPass(render_pass->_render_pass)
		.setPRasterizationState(&raster)
		.setPMultisampleState(&multisample)
		.setPDepthStencilState(&depth_stencil)
		.setPColorBlendState(&color_blend)
		.setPDynamicState(&dynamic_state)
		.setSubpass(0)
		.setLayout(_graphics_signature);

	VK_VALIDATE(_device.createGraphicsPipelines(vk::PipelineCache(nullptr), 1, &create_info, nullptr, &pipeline->_pipeline));

	return std::move(pipeline);
}

std::unique_ptr<st_pipeline> st_vk_graphics_context::create_compute_pipeline(const st_compute_state_desc& desc)
{
	std::unique_ptr<st_vk_pipeline> pipeline = std::make_unique<st_vk_pipeline>();
	pipeline->_device = &_device;

	const st_vk_shader* shader = static_cast<const st_vk_shader*>(desc._shader);

	vk::PipelineShaderStageCreateInfo stage = vk::PipelineShaderStageCreateInfo()
		.setStage(vk::ShaderStageFlagBits::eCompute)
		.setModule(shader->_cs)
		.setPName("cs_main");

	vk::ComputePipelineCreateInfo create_info = vk::ComputePipelineCreateInfo()
		.setStage(stage)
		.setLayout(_compute_signature);

	VK_VALIDATE(_device.createComputePipelines(vk::PipelineCache(nullptr), 1, &create_info, nullptr, &pipeline->_pipeline));

	return std::move(pipeline);
}

std::unique_ptr<st_vertex_format> st_vk_graphics_context::create_vertex_format(
	const struct st_vertex_attribute* attributes,
	uint32_t attribute_count)
{
	std::unique_ptr<st_vk_vertex_format> vertex_format = std::make_unique<st_vk_vertex_format>();
	vertex_format->_device = &_device;

	size_t vertex_size = calculate_vertex_size(attributes, attribute_count);
	vertex_format->_vertex_size = (uint32_t)vertex_size;

	// Create the element descriptions.
	size_t offset = 0;
	for (uint32_t itr = 0; itr < attribute_count; ++itr)
	{
		const st_vertex_attribute* attr = &attributes[itr];

		vk::VertexInputAttributeDescription attribute = vk::VertexInputAttributeDescription()
			.setBinding(0)
			.setFormat(convert_format(attr->_format))
			.setLocation(attr->_unit)
			.setOffset(offset);
		vertex_format->_attribute_descs.push_back(attribute);

		offset += bytes_per_pixel(attr->_format);
	}

	vk::VertexInputBindingDescription binding = vk::VertexInputBindingDescription()
		.setBinding(0)
		.setInputRate(vk::VertexInputRate::eVertex)
		.setStride(offset);

	vertex_format->_binding_descs.push_back(binding);

	vertex_format->_input_layout
		.setVertexBindingDescriptionCount(vertex_format->_binding_descs.size())
		.setVertexAttributeDescriptionCount(vertex_format->_attribute_descs.size())
		.setPVertexBindingDescriptions(vertex_format->_binding_descs.data())
		.setPVertexAttributeDescriptions(vertex_format->_attribute_descs.data());

	return std::move(vertex_format);
}

std::unique_ptr<st_render_pass> st_vk_graphics_context::create_render_pass(const st_render_pass_desc& desc)
{
	std::unique_ptr<st_vk_render_pass> render_pass = std::make_unique<st_vk_render_pass>();
	render_pass->_device = &_device;

	// Flip the Vulkan viewport to match the coordinate systems of OpenGL and DirectX.
	render_pass->_viewport = vk::Viewport()
		.setX(0)
		.setY(desc._viewport._height)
		.setWidth(desc._viewport._width)
		.setHeight(-desc._viewport._height)
		.setMinDepth(0.0f)
		.setMaxDepth(1.0f);

	std::vector<vk::AttachmentDescription> attachment_descs;
	std::vector<vk::AttachmentReference> attachment_refs;
	for (int i = 0; i < desc._attachment_count; ++i)
	{
		vk::AttachmentDescription att_desc = vk::AttachmentDescription()
			.setFormat(convert_format(desc._attachments[i]._format))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(convert_load_op(desc._attachments[i]._load_op))
			.setStoreOp(convert_store_op(desc._attachments[i]._store_op))
			.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachment_descs.push_back(att_desc);

		vk::AttachmentReference ref = vk::AttachmentReference()
			.setAttachment(i)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachment_refs.push_back(ref);
	}

	vk::SubpassDescription subpass_desc = vk::SubpassDescription()
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setInputAttachmentCount(0)
		.setColorAttachmentCount(desc._attachment_count)
		.setPColorAttachments(attachment_refs.data());

	vk::AttachmentReference ds_ref;
	if (desc._depth_attachment._format != st_format_unknown)
	{
		vk::AttachmentDescription ds_desc = vk::AttachmentDescription()
			.setFormat(convert_format(desc._depth_attachment._format))
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(convert_load_op(desc._depth_attachment._load_op))
			.setStoreOp(convert_store_op(desc._depth_attachment._store_op))
			.setStencilLoadOp(convert_load_op(desc._depth_attachment._load_op))
			.setStencilStoreOp(convert_store_op(desc._depth_attachment._store_op))
			.setInitialLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
		attachment_descs.push_back(ds_desc);

		ds_ref = vk::AttachmentReference()
			.setAttachment(attachment_descs.size() - 1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		subpass_desc.setPDepthStencilAttachment(&ds_ref);
	}

	vk::RenderPassCreateInfo pass_info = vk::RenderPassCreateInfo()
		.setAttachmentCount(attachment_descs.size())
		.setPAttachments(attachment_descs.data())
		.setSubpassCount(1)
		.setPSubpasses(&subpass_desc)
		.setDependencyCount(0);

	VK_VALIDATE(_device.createRenderPass(&pass_info, nullptr, &render_pass->_render_pass));

	return std::move(render_pass);
}

void st_vk_graphics_context::begin_render_pass(
	st_render_pass* pass_,
	st_framebuffer* framebuffer_,
	const st_clear_value* clear_values,
	const uint8_t clear_count)
{
	st_vk_render_pass* pass = static_cast<st_vk_render_pass*>(pass_);
	st_vk_framebuffer* framebuffer = static_cast<st_vk_framebuffer*>(framebuffer_);

	for (uint32_t i = 0; i < framebuffer->_targets.size(); ++i)
	{
		transition(framebuffer->_targets[i], st_texture_state_render_target);
	}
	if (framebuffer->_depth_stencil)
	{
		transition(framebuffer->_depth_stencil, st_texture_state_depth_stencil_target);
	}

	std::vector<vk::ClearValue> clears;
	clears.reserve(clear_count);
	for (uint32_t c_itr = 0; c_itr < clear_count; ++c_itr)
	{
		if (c_itr < framebuffer->_targets.size())
		{
			std::array<float, 4> c;
			std::copy(clear_values[c_itr]._color.axes, clear_values[c_itr]._color.axes + 4, c.begin());
			clears.push_back(vk::ClearValue().setColor(vk::ClearColorValue().setFloat32(c)));
		}
		else
		{
			vk::ClearDepthStencilValue ds_clear = vk::ClearDepthStencilValue()
				.setDepth(clear_values[c_itr]._depth_stencil._depth)
				.setStencil(clear_values[c_itr]._depth_stencil._stencil);
			clears.push_back(vk::ClearValue().setDepthStencil(ds_clear));
		}
	}

	vk::RenderPassBeginInfo begin_info = vk::RenderPassBeginInfo()
		.setPClearValues(clears.data())
		.setClearValueCount(clears.size())
		.setFramebuffer(framebuffer->_framebuffer)
		.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(framebuffer->_width, framebuffer->_height)))
		.setRenderPass(pass->_render_pass);

	_command_buffers[st_command_buffer_graphics].beginRenderPass(&begin_info, vk::SubpassContents::eInline);
	_command_buffers[st_command_buffer_graphics].setViewportWithCount(1, &pass->_viewport);
}

void st_vk_graphics_context::end_render_pass(st_render_pass* pass, st_framebuffer* framebuffer_)
{
	_command_buffers[st_command_buffer_graphics].endRenderPass();
}

std::unique_ptr<st_framebuffer> st_vk_graphics_context::create_framebuffer(const st_framebuffer_desc& desc)
{
	std::unique_ptr<st_vk_framebuffer> framebuffer = std::make_unique<st_vk_framebuffer>();
	framebuffer->_device = &_device;

	uint32_t attachmentCount = desc._target_count + (desc._depth_target._texture ? 1 : 0);

	if (desc._target_count > 0)
	{
		st_vk_texture* t = static_cast<st_vk_texture*>(desc._targets[0]._texture);
		framebuffer->_width = t->_width;
		framebuffer->_height = t->_height;
	}
	else if (desc._depth_target._texture)
	{
		st_vk_texture* ds = static_cast<st_vk_texture*>(desc._depth_target._texture);
		framebuffer->_width = ds->_width;
		framebuffer->_height = ds->_height;
	}

	std::vector<vk::ImageView> views;
	views.reserve(attachmentCount);

	for (int i = 0; i < desc._target_count; ++i)
	{
		const st_vk_texture_view* view = static_cast<const st_vk_texture_view*>(desc._targets[i]._view);

		views.push_back(view->_view);
		framebuffer->_targets.push_back(desc._targets[i]._texture);
	}

	if (desc._depth_target._texture)
	{
		const st_vk_texture_view* ds = static_cast<const st_vk_texture_view*>(desc._depth_target._view);

		views.push_back(ds->_view);
		framebuffer->_depth_stencil = desc._depth_target._texture;
	}

	st_vk_render_pass* pass = static_cast<st_vk_render_pass*>(desc._pass);

	vk::FramebufferCreateInfo create_info = vk::FramebufferCreateInfo()
		.setAttachmentCount(attachmentCount)
		.setPAttachments(views.data())
		.setRenderPass(pass->_render_pass)
		.setWidth(framebuffer->_width)
		.setHeight(framebuffer->_height)
		.setLayers(1);

	VK_VALIDATE(_device.createFramebuffer(&create_info, nullptr, &framebuffer->_framebuffer));

	return std::move(framebuffer);
}

void st_vk_graphics_context::get_desc(const st_texture* texture_, st_texture_desc* out_desc)
{
	assert(out_desc);
	const st_vk_texture* texture = static_cast<const st_vk_texture*>(texture_);
	out_desc->_format = texture->_format;
	out_desc->_width = texture->_width;
	out_desc->_height = texture->_height;
	out_desc->_levels = texture->_levels;
	out_desc->_usage = texture->_usage;
	// TODO: Depth and others.
}

void st_vk_graphics_context::get_supported_formats(
	const st_window* window,
	std::vector<e_st_format>& formats)
{
	formats.push_back(st_format_r8g8b8a8_unorm);
	formats.push_back(st_format_r8g8b8a8_unorm_srgb);

	vk::Win32SurfaceCreateInfoKHR win32_surface_info = vk::Win32SurfaceCreateInfoKHR()
		.setHinstance(GetModuleHandle(NULL))
		.setHwnd((HWND)window->get_window_handle());

	// TODO: This surface is already on the swap chain, but use of this function predates
	// swap chain creation. The surface best belongs to a platform-specific window/graphics
	// interface type, which for the vulkan backend would be all of the win32/KHR functions.
	// On DX, as an alternative example, it would be the IDXGI* types.
	vk::SurfaceKHR surface;
	VK_VALIDATE(_instance.createWin32SurfaceKHR(&win32_surface_info, nullptr, &surface));

	uint32_t surface_format_count;
	VK_VALIDATE(_gpu.getSurfaceFormatsKHR(surface, &surface_format_count, nullptr));

	std::vector<vk::SurfaceFormatKHR> surface_formats;
	surface_formats.resize(surface_format_count);
	VK_VALIDATE(_gpu.getSurfaceFormatsKHR(surface, &surface_format_count, surface_formats.data()));

	if (std::find_if(surface_formats.begin(), surface_formats.end(), [](const vk::SurfaceFormatKHR& format)
		{
			return format.format == vk::Format::eA2B10G10R10UnormPack32;
		}) != surface_formats.end())
	{
		// TODO: Vulkan has an issue where getSurfaceFormatsKHR always returns an HDR format
		// even if the OS HDR setting is not on.
		//formats.push_back(st_format_r10g10b10a2_unorm);
	}

	_instance.destroySurfaceKHR(surface, nullptr);
}

#endif
