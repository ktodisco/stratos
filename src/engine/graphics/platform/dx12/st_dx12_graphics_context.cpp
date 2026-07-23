/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics_context.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <core/st_core.h>

#include <graphics/platform/dx12/st_dx12_conversion.h>
#include <graphics/platform/dx12/st_dx12_device.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/st_drawcall.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_render_texture.h>
#include <graphics/st_shader_manager.h>
#include <graphics/st_texture_loader.h>

#include <system/st_window.h>

#include <cassert>
#include <cstdio>
#include <vector>

#include <d3dcompiler.h>

extern char g_root_path[256];

st_dx12_graphics_context::st_dx12_graphics_context(const st_window* window)
{
	UINT dxgi_factory_flags = 0;
#if defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debug_controller;
		if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debug_controller)))
		{
			debug_controller->EnableDebugLayer();

			// Enable additional debug layers.
			dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	HRESULT result = CreateDXGIFactory2(dxgi_factory_flags, __uuidof(IDXGIFactory4), (void**)_dxgi_factory.GetAddressOf());

	if (result != S_OK)
	{
		assert(false);
	}
}

st_dx12_graphics_context::~st_dx12_graphics_context()
{
	_dxgi_factory = nullptr;

#if defined(_DEBUG)
	{
		HMODULE debug_module = GetModuleHandle("Dxgidebug.dll");
		if (debug_module)
		{
			typedef HRESULT (WINAPI* LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
			auto dxgiGetDebugInterface =
				reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(GetProcAddress(debug_module, "DXGIGetDebugInterface"));

			Microsoft::WRL::ComPtr<IDXGIDebug> dxgi_debug;
			if (SUCCEEDED(dxgiGetDebugInterface(__uuidof(IDXGIDebug), (void**)&dxgi_debug)))
			{
				dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			}
		}
	}
#endif
}

std::unique_ptr<st_device> st_dx12_graphics_context::create_device(const st_device_desc& desc)
{
	// Enumerate the adapters in the machine.
	std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (uint32_t adapter_itr = 0;
		_dxgi_factory->EnumAdapters1(adapter_itr, &adapter) != DXGI_ERROR_NOT_FOUND;
		++adapter_itr)
	{
		adapters.push_back(adapter);
	}

	// Choose the adapter with the most dedicated memory.
	Microsoft::WRL::ComPtr<IDXGIAdapter1> hardware_adapter;
	size_t max_memory = 0;
	for (uint32_t adapter_itr = 0; adapter_itr < adapters.size(); ++adapter_itr)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapters[adapter_itr]->GetDesc1(&desc);

		if (desc.DedicatedVideoMemory > max_memory);
		{
			hardware_adapter = adapters[adapter_itr];
			max_memory = desc.DedicatedVideoMemory;

			// Naively take the first one. At the time of development, this is commonly
			// the dedicated graphics card.
			break;
		}
	}

	Microsoft::WRL::ComPtr<ID3D12Device> d3d_device;

	// Create the device and command queue.
	HRESULT result = D3D12CreateDevice(
		hardware_adapter.Get(),
		D3D_FEATURE_LEVEL_12_1,
		__uuidof(ID3D12Device),
		(void**)&d3d_device);

	std::unique_ptr<st_dx12_device> device = std::make_unique<st_dx12_device>(d3d_device.Get(), this);

	if (result != S_OK)
	{
		assert(false);
	}

	return std::move(device);
}

void st_dx12_graphics_context::get_supported_formats(
	const st_window* window,
	st_device* device,
	std::vector<e_st_format>& formats)
{
	// These formats are always supported.
	formats.push_back(st_format_r8g8b8a8_unorm);
	formats.push_back(st_format_r8g8b8a8_unorm_srgb);

	// Test for HDR support.
    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgi_adapter;
    HRESULT result = _dxgi_factory->EnumAdapters1(0, &dxgi_adapter);

	Microsoft::WRL::ComPtr<IDXGIOutput> current_output;
    Microsoft::WRL::ComPtr<IDXGIOutput> best_output;
    float best_intersect_area = -1;

	RECT window_bounds;
	HWND window_handle = window->get_window_handle();
	GetWindowRect(window_handle, &window_bounds);

	auto compute_intersection_area = [](int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return max(0, min(ax2, bx2) - max(ax1, bx1)) * max(0, min(ay2, by2) - max(ay1, by1));
	};

	uint32_t i = 0;
    while (dxgi_adapter->EnumOutputs(i, &current_output) != DXGI_ERROR_NOT_FOUND)
    {
        int ax1 = window_bounds.left;
        int ay1 = window_bounds.top;
        int ax2 = window_bounds.right;
        int ay2 = window_bounds.bottom;

        DXGI_OUTPUT_DESC desc;
        result = current_output->GetDesc(&desc);
        RECT r = desc.DesktopCoordinates;
        int bx1 = r.left;
        int by1 = r.top;
        int bx2 = r.right;
        int by2 = r.bottom;

        int intersect_area = compute_intersection_area(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
        if (intersect_area > best_intersect_area)
        {
            best_output = current_output;
            best_intersect_area = static_cast<float>(intersect_area);
        }

        i++;
    }

    Microsoft::WRL::ComPtr<IDXGIOutput6> output6;
    result = best_output.As(&output6);

    DXGI_OUTPUT_DESC1 desc1;
    result = output6->GetDesc1(&desc1);

    if (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
		formats.push_back(st_format_r10g10b10a2_unorm);
}

#endif
