#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

enum e_st_properties
{
	st_property_invalid,
	st_property_position_x,
	st_property_position_y,
	st_property_position_z,
	st_property_normal_x,
	st_property_normal_y,
	st_property_normal_z,
	st_property_uv_s,
	st_property_uv_t,
	st_property_count,
};

struct st_ply_parser_state
{
	int _vertex_count = 0;
	int _face_count = 0;
	int _property_count = 0;
	e_st_properties _properties[st_property_count] = { st_property_invalid };
};

/*
** Read a PLY file, get the model data.
*/
void ply_to_model(const char* filename, struct st_model_data* model);
