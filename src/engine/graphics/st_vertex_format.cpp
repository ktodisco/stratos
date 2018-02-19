/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_vertex_format.h>

st_vertex_format::st_vertex_format()
{
}

st_vertex_format::~st_vertex_format()
{
}

const st_vertex_attribute& st_vertex_format::get_attribute(uint32_t index) const
{
	return _attributes[index];
}

void st_vertex_format::add_attribute(st_vertex_attribute& attribute)
{
	_attributes.push_back(attribute);
}

void st_vertex_format::finalize()
{
	size_t vertex_size = 0;

	for (uint32_t itr = 0; itr < _attributes.size(); ++itr)
	{
		st_vertex_attribute* attr = &_attributes[itr];

		// TODO: Switch on the attribute data type.
		size_t data_size = sizeof(float);

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
		case st_vertex_attribute_normal:
		case st_vertex_attribute_binormal:
		case st_vertex_attribute_tangent:
			vertex_size += data_size * 3;
			break;
		case st_vertex_attribute_color:
		case st_vertex_attribute_joints:
		case st_vertex_attribute_weights:
			vertex_size += data_size * 4;
			break;
		case st_vertex_attribute_uv:
			vertex_size += data_size * 2;
			break;
		default:
			// TODO: Assert.
			break;
		}
	}

	_vertex_size = (uint32_t)vertex_size;

	platform_finalize();
}
