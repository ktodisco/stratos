#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_graphics_context.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <cstdint>
#include <string>

class st_dx12_graphics_context : public st_graphics_context
{
public:

	st_dx12_graphics_context(const class st_window* window);
	~st_dx12_graphics_context();

	std::unique_ptr<st_device> create_device(const st_device_desc& desc) override;

	// Informational.
	e_st_graphics_api get_api() { return e_st_graphics_api::dx12; }
	void get_supported_formats(
		const class st_window* window,
		class st_device* device,
		std::vector<e_st_format>& formats) override;

	// API-specific.
	IDXGIFactory4* get_dxgi_factory() { return _dxgi_factory.Get(); }

private:

	Microsoft::WRL::ComPtr<IDXGIFactory4> _dxgi_factory;
};

#endif
