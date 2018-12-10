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
	void set_meta(const char* name) {}

	void bind(class st_dx12_render_context* context);

	void set_name(std::string name);

	ID3D12Resource* get_resource() const { return _handle.Get(); }
	e_st_texture_format get_format() const { return _format; }

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> _handle;

	e_st_texture_format _format;
	uint32_t _sampler;
	uint32_t _srv;

	friend class st_dx12_render_texture;
};

#endif
