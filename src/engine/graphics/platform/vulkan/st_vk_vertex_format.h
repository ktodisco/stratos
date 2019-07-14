#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>
#include <vector>

class st_vk_vertex_format
{
public:
	st_vk_vertex_format() {}
	~st_vk_vertex_format() {}

	virtual uint32_t get_attribute_count() const = 0;
	virtual const class st_vertex_attribute& get_attribute(uint32_t index) const = 0;

	void platform_finalize() {}

	//const D3D12_INPUT_LAYOUT_DESC& get_layout() const { return _input_layout; }

private:
	//std::vector<D3D12_INPUT_ELEMENT_DESC> _elements;
	//D3D12_INPUT_LAYOUT_DESC _input_layout = {};
};

#endif
