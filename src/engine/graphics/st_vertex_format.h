#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/st_vertex_attribute.h"

#include <cstdint>
#include <vector>

class st_vertex_format
{
public:

	st_vertex_format();
	~st_vertex_format();

	uint32_t get_attribute_count() const
	{
		return static_cast<uint32_t>(_attributes.size());
	}

	uint32_t get_vertex_size() const
	{
		return _vertex_size;
	}

	const st_vertex_attribute& get_attribute(uint32_t index) const;

	void add_attribute(st_vertex_attribute& attribute);
	void finalize();

private:

	std::vector<st_vertex_attribute> _attributes;
	uint32_t _vertex_size = 0;
};
