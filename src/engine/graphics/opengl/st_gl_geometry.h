#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>

class st_vertex_format;

class st_gl_geometry
{
public:

	st_gl_geometry(
		const st_vertex_format* format,
		void* vertex_data,
		uint32_t vertex_size,
		uint32_t vertex_count,
		uint16_t* index_data,
		uint32_t index_count);
	~st_gl_geometry();

	void draw(struct st_gl_static_drawcall& draw_call);

private:

	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
};
