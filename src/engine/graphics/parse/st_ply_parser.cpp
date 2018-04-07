/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/parse/st_ply_parser.h>

#include <graphics/geometry/st_model_data.h>
#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/geometry/st_vertex_format.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

void ply_to_model(const char* filename, struct st_model_data* model)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += filename;

	std::ifstream file(fullpath);

	assert(file.is_open());

	st_ply_parser_state state;

	// Read in the header.
	char data[128];

	file >> data;
	while (strcmp(data, "element") != 0)
	{
		file >> data;
	}

	file >> data;
	if (strcmp(data, "vertex") == 0)
	{
		file >> data;
		state._vertex_count = atoi(data);
	}

	file >> data;
	while (strcmp(data, "property") != 0)
	{
		file >> data;
	}

	while (strcmp(data, "property") == 0)
	{
		// Type (float, int, etc.)
		file >> data;

		// Identifier.
		file >> data;

		if (strcmp(data, "x") == 0) state._properties[state._property_count] = st_property_position_x;
		if (strcmp(data, "y") == 0) state._properties[state._property_count] = st_property_position_y;
		if (strcmp(data, "z") == 0) state._properties[state._property_count] = st_property_position_z;
		if (strcmp(data, "nx") == 0) state._properties[state._property_count] = st_property_normal_x;
		if (strcmp(data, "ny") == 0) state._properties[state._property_count] = st_property_normal_y;
		if (strcmp(data, "nz") == 0) state._properties[state._property_count] = st_property_normal_z;
		if (strcmp(data, "s") == 0) state._properties[state._property_count] = st_property_uv_s;
		if (strcmp(data, "t") == 0) state._properties[state._property_count] = st_property_uv_t;

		state._property_count++;

		// Next property tag?
		file >> data;
	}

	if (strcmp(data, "element") == 0)
	{
		file >> data;
		file >> data;
		state._face_count = atoi(data);
	}

	// Read to the end of the header.
	file >> data;
	while (strcmp(data, "end_header") != 0)
	{
		file >> data;
	}

	// Start collecting property data.
	for (int i = 0; i < state._vertex_count; ++i)
	{
		st_vertex vertex;

		for (int p = 0; p < state._property_count; ++p)
		{
			// Property value.
			file >> data;

			switch (state._properties[p])
			{
			case st_property_position_x:
				vertex._position.x = (float)atof(data);
				break;
			case st_property_position_y:
				vertex._position.y = (float)atof(data);
				break;
			case st_property_position_z:
				vertex._position.z = (float)atof(data);
				break;
			case st_property_normal_x:
				vertex._normal.x = (float)atof(data);
				break;
			case st_property_normal_y:
				vertex._normal.y = (float)atof(data);
				break;
			case st_property_normal_z:
				vertex._normal.z = (float)atof(data);
				break;
			case st_property_uv_s:
				vertex._uv.x = (float)atof(data);
				break;
			case st_property_uv_t:
				vertex._uv.y = (float)atof(data);
				break;
			default:
				break;
			}
		}

		model->_vertices.push_back(vertex);
	}

	// Collect all the face data.
	for (int i = 0; i < state._face_count; ++i)
	{
		file >> data;
		int index_count = atoi(data);

		std::vector<uint32_t> indices;
		for (int c = 0; c < index_count; ++c)
		{
			file >> data;
			indices.push_back(atoi(data));
		}

		model->_indices.push_back(indices[0]);
		model->_indices.push_back(indices[1]);
		model->_indices.push_back(indices[2]);

		if (index_count > 3)
		{
			model->_indices.push_back(indices[0]);
			model->_indices.push_back(indices[2]);
			model->_indices.push_back(indices[3]);
		}
	}

	// If the model did not contain normals, we'll have to calculate them manually.
	bool has_normals = false;
	for (int i = 0; i < st_property_count; ++i)
	{
		if (state._properties[i] == st_property_normal_x ||
			state._properties[i] == st_property_normal_y ||
			state._properties[i] == st_property_normal_z)
		{
			has_normals = true;
			break;
		}
	}

	if (!has_normals)
	{
		std::vector<int> cont;
		for (int i = 0; i < model->_vertices.size(); ++i)
		{
			cont.push_back(0);
		}

		// Calculate the normals.
		for (int i = 0; i < model->_indices.size(); i += 3)
		{
			uint32_t index0 = model->_indices[i];
			uint32_t index1 = model->_indices[i + 1];
			uint32_t index2 = model->_indices[i + 2];

			st_vec3f v0 = model->_vertices[index1]._position - model->_vertices[index0]._position;
			st_vec3f v1 = model->_vertices[index2]._position - model->_vertices[index0]._position;

			st_vec3f normal = st_vec3f_cross(v0, v1).normal();

			model->_vertices[index0]._normal += normal; cont[index0]++;
			model->_vertices[index1]._normal += normal; cont[index1]++;
			model->_vertices[index2]._normal += normal; cont[index2]++;
		}

		for (int i = 0; i < model->_vertices.size(); ++i)
		{
			model->_vertices[i]._normal.normalize();
		}
	}

	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_normal, 1));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_color, 2));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 3));

	model->_vertex_format.finalize();
}