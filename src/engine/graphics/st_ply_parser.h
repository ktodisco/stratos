#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

struct st_ply_parser_state
{
	int _vertex_count = 0;
	int _face_count = 0;
	int _property_count = 0;
};

/*
** Read a PLY file, get the model data.
*/
void ply_to_model(const char* filename, struct st_model* model);
