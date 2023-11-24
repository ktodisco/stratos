#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)
#include <graphics/platform/opengl/st_gl_geometry.h>

typedef st_gl_geometry st_platform_geometry;
#elif defined(ST_GRAPHICS_API_DX12)
#include <graphics/platform/dx12/st_dx12_geometry.h>

typedef st_dx12_geometry st_platform_geometry;
#elif defined(ST_GRAPHICS_API_VULKAN)
#include <graphics/platform/vulkan/st_vk_geometry.h>

typedef st_vk_geometry st_platform_geometry;
#else
#error Graphics API not defined.
#endif

class st_geometry : public st_platform_geometry
{
public:

	st_geometry(
		const st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);

private:
	std::unique_ptr<st_buffer> _vertex_buffer;
	std::unique_ptr<st_buffer> _index_buffer;

	std::unique_ptr<st_buffer_view> _vertex_buffer_view;
	std::unique_ptr<st_buffer_view> _index_buffer_view;

	uint32_t _index_count = 0;
};
