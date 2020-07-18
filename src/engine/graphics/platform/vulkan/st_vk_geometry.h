#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_VULKAN)

#include <cstdint>

class st_vertex_format;

class st_vk_geometry
{
public:

	st_vk_geometry(
		const class st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);
	~st_vk_geometry();

	void draw(struct st_static_drawcall& draw_call);

private:
	vk::Buffer _vertex_buffer;
	vk::Buffer _index_buffer;
	uint32_t _index_count = 0;
};

#endif
