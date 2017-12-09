/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_scene_render_pass.h"

#include "framework/st_drawcall.h"
#include "framework/st_frame_params.h"
#include "graphics/st_material.h"
#include "graphics/st_render_marker.h"

st_scene_render_pass::st_scene_render_pass()
{
	_default_material = new st_constant_color_material();
	_default_material->init();
}

st_scene_render_pass::~st_scene_render_pass()
{
	delete _default_material;
}

void st_scene_render_pass::render(const st_frame_params* params)
{
	st_render_marker marker("st_scene_render_pass::render");

	// Compute projection matrices:
	st_mat4f perspective;
	perspective.make_perspective_rh(st_degrees_to_radians(45.0f), (float)params->_width / (float)params->_height, 0.1f, 10000.0f);

	// Draw all static geometry:
	for (auto& d : params->_static_drawcalls)
	{
		st_render_marker draw_marker(d._name.c_str());

		d._material->bind(perspective, params->_view, d._transform);
		glBindVertexArray(d._vao);
		glDrawElements(d._draw_mode, d._index_count, GL_UNSIGNED_SHORT, 0);
	}

	// Draw all dynamic geometry:
	draw_dynamic(params->_dynamic_drawcalls, perspective, params->_view);
}

void st_scene_render_pass::draw_dynamic(
	const std::vector<st_dynamic_drawcall>& drawcalls,
	const class st_mat4f& proj,
	const class st_mat4f& view)
{
	for (auto& d : drawcalls)
	{
		st_render_marker draw_marker(d._name);

		if (d._material)
		{
			d._material->set_color(d._color);
			d._material->bind(proj, view, d._transform);
		}
		else
		{
			_default_material->set_color(d._color);
			_default_material->bind(proj, view, d._transform);
		}

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GLuint pos;
		glGenBuffers(1, &pos);
		glBindBuffer(GL_ARRAY_BUFFER, pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec3f) * d._positions.size(), &d._positions[0], GL_STREAM_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		GLuint texcoord;
		if (!d._texcoords.empty())
		{
			glGenBuffers(1, &texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(st_vec2f) * d._texcoords.size(), &d._texcoords[0], GL_STREAM_DRAW);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		}

		GLuint indices;
		glGenBuffers(1, &indices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * d._indices.size(), &d._indices[0], GL_STREAM_DRAW);

		glDrawElements(d._draw_mode, (GLsizei)d._indices.size(), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDeleteBuffers(1, &indices);
		if (!d._texcoords.empty())
		{
			glDeleteBuffers(1, &texcoord);
		}
		glDeleteBuffers(1, &pos);
		glDeleteVertexArrays(1, &vao);
		glBindVertexArray(0);
	}
}
