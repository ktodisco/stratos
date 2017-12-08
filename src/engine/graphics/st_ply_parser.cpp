/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_ply_parser.h"

#include "st_geometry.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

void ply_to_model(const char* filename, struct st_model* model)
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
	if (strcmp(data, "property") == 0)
	{
		while (strcmp(data, "property") == 0)
		{
			file >> data;
			file >> data;
			state._property_count++;
			file >> data;
		}
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

		file >> data; vertex._position.x = (float)atof(data);
		file >> data; vertex._position.y = (float)atof(data);
		file >> data; vertex._position.z = (float)atof(data);
		file >> data;
		file >> data;

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

	model->_vertex_format |= k_vertex_attribute_normal;
}