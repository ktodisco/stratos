#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_DX12)

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>

#include <cstdint>
#include <vector>

class st_dx12_resource_table
{
public:
	st_dx12_resource_table();
	~st_dx12_resource_table();

	void add_constant_buffer(class st_dx12_constant_buffer* cb);
	void add_texture_resource(class st_dx12_texture* sr);
	void add_buffer_resource(class st_dx12_buffer* br);

	void bind(class st_dx12_render_context* context);

private:
	std::vector<st_dx12_descriptor> _cbvs;
	std::vector<st_dx12_descriptor> _srvs;
	std::vector<st_dx12_descriptor> _samplers;
	std::vector<st_dx12_descriptor> _buffers;
};

#endif
