#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <math/st_mat4f.h>
#include <math/st_vec2f.h>
#include <math/st_vec3f.h>

#include <cstdint>
#include <string>
#include <vector>

/*
** A draw emitted from the simulation phase and rendered in the output phase.
** @see st_frame_params
*/
struct st_gl_drawcall
{
	std::string _name;
	st_mat4f _transform;
	e_st_primitive_topology _draw_mode;
	class st_material* _material = 0;
};

/*
** Draw call with static geometry.
** The vertex array object referenced by this draw call should live for at
** least several frames, and probably longer.
*/
struct st_gl_static_drawcall : st_gl_drawcall
{
	uint32_t _vao;
	int32_t _index_count;
};

/*
** Draw call with dynamic geometry.
** Geometry referenced by this draw call should only a single frame.
*/
struct st_gl_dynamic_drawcall : st_gl_drawcall
{
	std::vector<st_vec3f> _positions;
	std::vector<st_vec2f> _texcoords;
	std::vector<uint16_t> _indices;
	st_vec3f _color;
};

#endif
