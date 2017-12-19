#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <cstdint>
#include <functional>

const int k_invalid_vertex_index = 0xffffffff;

/*
** State related to parsing data.
*/
struct st_egg_parser_state
{
	uint32_t _first_vertex_index = k_invalid_vertex_index;
	uint32_t _vertex_format = 0;
	std::function<void(struct st_vec3f&)> _vector_coordinate_conversion;
	std::function<void(struct st_mat4f&)> _matrix_coordinate_conversion;
};

/*
** Read an EGG file, get the model data.
*/
void egg_to_model(const char* filename, struct st_model* model);

/*
** Read an EGG file, get the animation data.
*/
void egg_to_animation(const char* filename, struct st_animation* animation, struct st_model* model);
