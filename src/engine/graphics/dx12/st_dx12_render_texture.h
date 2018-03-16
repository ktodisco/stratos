#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/st_texture.h>

#include <math/st_vec4f.h>

#include <cstdint>
#include <memory>

class st_dx12_render_texture : public st_texture
{
public:
	st_dx12_render_texture(
		uint32_t width,
		uint32_t height,
		e_st_texture_format format,
		const st_vec4f& clear);
	~st_dx12_render_texture();

	uint32_t get_rtv_offset() const { return _rtv; }

private:
	uint32_t _rtv;
};

#endif
