#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <string>
#include <vector>

class st_gl_constant_buffer
{
public:
	st_gl_constant_buffer(const size_t size);
	~st_gl_constant_buffer();

	void add_constant(
		const std::string& name,
		const e_st_shader_constant_type constant_type);

	void update(const class st_gl_render_context* context, void* data);

	void commit(class st_gl_render_context* context);

private:
	struct st_gl_constant
	{
		std::string _name;
		e_st_shader_constant_type _type;
	};

	std::vector<st_gl_constant> _constants;
};

#endif
