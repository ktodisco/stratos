/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/vulkan/st_vk_graphics_context.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <graphics/platform/vulkan/st_vk_conversion.h>
#include <graphics/platform/vulkan/st_vk_device.h>

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
}

st_vk_graphics_context::~st_vk_graphics_context()
{
	_instance.destroy(nullptr);

	FreeLibrary((HMODULE)_vk_library);
}

std::unique_ptr<st_device> st_vk_graphics_context::create_device(const st_device_desc& desc)
{
	std::unique_ptr<st_vk_device> device = std::make_unique<st_vk_device>(desc, &_instance);
	return std::move(device);
}

void st_vk_graphics_context::get_supported_formats(
	const st_window* window,
	st_device* device_,
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

	st_vk_device* device = static_cast<st_vk_device*>(device_);

	uint32_t surface_format_count;
	VK_VALIDATE(device->get_physical_device()->getSurfaceFormatsKHR(surface, &surface_format_count, nullptr));

	std::vector<vk::SurfaceFormatKHR> surface_formats;
	surface_formats.resize(surface_format_count);
	VK_VALIDATE(device->get_physical_device()->getSurfaceFormatsKHR(surface, &surface_format_count, surface_formats.data()));

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
