#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>
#include <string>
#include <Windows.h>
#include <wrl.h>

class st_dx12_texture
{
public:
	st_dx12_texture();
	~st_dx12_texture();

	void load_from_data(uint32_t width, uint32_t height, e_st_texture_format format, void* data);
	bool load_from_file(const char* path);
	void set_meta(const char* name, uint32_t slot) {}

	void bind(class st_dx12_render_context* context);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	uint32_t _sampler;
	uint32_t _srv;
};

#endif