#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <cstdint>

class st_dx12_resource_table
{
public:
	st_dx12_resource_table();
	~st_dx12_resource_table();

	void add_constant_buffer(class st_dx12_constant_buffer* cb);
	void add_shader_resource(class st_dx12_texture* sr);

	void bind(class st_dx12_render_context* context);

private:
	uint32_t _cbv_srv_offset = UINT_MAX;
	// TODO: This is a total hack.
	uint32_t _cbv_count = 0;
	uint32_t _sampler_offset = UINT_MAX;
};

#endif
