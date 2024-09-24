#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <cstdint>
#include <memory>
#include <vector>

class st_geometry
{
public:

	st_geometry(
		const struct st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);
	~st_geometry();

	void draw(struct st_static_drawcall& draw_call);

private:
	std::unique_ptr<struct st_buffer> _vertex_buffer;
	std::unique_ptr<struct st_buffer> _index_buffer;
	uint32_t _index_count = 0;
};
