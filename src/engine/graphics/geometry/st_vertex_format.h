#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_vertex_format.h>

typedef st_gl_vertex_format st_platform_vertex_format;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_vertex_format.h>

typedef st_dx12_vertex_format st_platform_vertex_format;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_vertex_format.h>

typedef st_vk_vertex_format st_platform_vertex_format;
#else
#error Graphics API not defined.
#endif

#include <graphics/geometry/st_vertex_attribute.h>

#include <cstdint>
#include <vector>

class st_vertex_format : public st_platform_vertex_format
{
public:

	st_vertex_format();
	~st_vertex_format();

	uint32_t get_attribute_count() const override
	{
		return static_cast<uint32_t>(_attributes.size());
	}

	const st_vertex_attribute& get_attribute(uint32_t index) const override;

	uint32_t get_vertex_size() const
	{
		return _vertex_size;
	}

	// A format is finalized when its vertex size is filled out.
	bool is_finalized() const
	{
		return get_vertex_size() > 0;
	}

	void add_attribute(st_vertex_attribute& attribute);
	void finalize();

private:
	std::vector<st_vertex_attribute> _attributes;
	uint32_t _vertex_size = 0;
};
