#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <graphics/st_drawcall.h>

class st_geometry
{
public:

	st_geometry(
		const st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);
	~st_geometry();

	void draw(st_static_drawcall& draw_call);

private:
	std::unique_ptr<st_buffer> _vertex_buffer;
	std::unique_ptr<st_buffer> _index_buffer;

	std::unique_ptr<st_buffer_view> _vertex_buffer_view;
	std::unique_ptr<st_buffer_view> _index_buffer_view;

	uint32_t _index_count = 0;
};
