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

	// Now, calculate the tangent vectors.
	// http://www.terathon.com/code/tangent.html
	st_vec3f* tan1 = new st_vec3f[model->_vertices.size()];
	memset(tan1, 0, sizeof(st_vec3f) * model->_vertices.size());

	for (uint32_t i = 0; i < model->_indices.size(); i += 3)
	{
		uint16_t i0 = model->_indices[i];
		uint16_t i1 = model->_indices[i + 1];
		uint16_t i2 = model->_indices[i + 2];

		const st_vec3f& v0 = model->_vertices[i0]._position;
		const st_vec3f& v1 = model->_vertices[i1]._position;
		const st_vec3f& v2 = model->_vertices[i2]._position;

		const st_vec2f& w0 = model->_vertices[i0]._uv;
		const st_vec2f& w1 = model->_vertices[i1]._uv;
		const st_vec2f& w2 = model->_vertices[i2]._uv;

		float x0 = v1.x - v0.x;
		float x1 = v2.x - v0.x;
		float y0 = v1.y - v0.y;
		float y1 = v2.y - v0.y;
		float z0 = v1.z - v0.z;
		float z1 = v2.z - v0.z;

		float s0 = w1.x - w0.x;
		float s1 = w2.x - w0.x;
		float t0 = w1.y - w0.y;
		float t1 = w2.y - w0.y;

		float r = 1.0f / (s0 * t1 - s1 * t0);
		st_vec3f s_dir
		{
			(t1 * x0 - t0 * x1) * r,
			(t1 * y0 - t0 * y1) * r,
			(t1 * z0 - t0 * z1) * r
		};
		st_vec3f t_dir
		{
			(s0 * x1 - s1 * x0) * r,
			(s0 * y1 - s1 * y0) * r,
			(s0 * z1 - s1 * z0) * r
		};

		tan1[i0] += s_dir;
		tan1[i1] += s_dir;
		tan1[i2] += s_dir;
	}

	for (uint32_t i = 0; i < model->_vertices.size(); ++i)
	{
		const st_vec3f& n = model->_vertices[i]._normal;
		const st_vec3f& t = tan1[i];

		// Gram-Schmidt orthogonalize.
		model->_vertices[i]._tangent = (t - n.scale_result(n.dot(t))).normal();
	}

	delete[] tan1;

	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_position, 0));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_normal, 1));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_color, 2));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_uv, 3));
	model->_vertex_format.add_attribute(st_vertex_attribute(st_vertex_attribute_tangent, 4));

	model->_vertex_format.finalize();
}