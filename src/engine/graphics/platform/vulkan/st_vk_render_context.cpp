/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_render_context.h>

#include <iostream>
#include <vector>

#if defined(ST_GRAPHICS_API_VULKAN)

st_vk_render_context::st_vk_render_context(const class st_window* window)
{
	std::vector<const char*> layer_names;
	layer_names.push_back("VK_LAYER_KHRONOS_validation");

	// Create the per-application instance object.
	auto app_info = vk::ApplicationInfo()
		.setPApplicationName("Stratos Renderer")
		.setPEngineName("Stratos")
		.setEngineVersion(1);

	auto create_info = vk::InstanceCreateInfo()
		.setPApplicationInfo(&app_info)
		.setEnabledLayerCount(0)
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
}

st_vk_render_context::~st_vk_render_context()
{
	_device.waitIdle();
	_device.destroy(nullptr);

	_instance.destroy(nullptr);
}

#endif
