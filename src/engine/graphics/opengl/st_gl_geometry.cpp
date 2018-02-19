/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_geometry.h>

#include <graphics/opengl/st_gl_drawcall.h>
#include <graphics/st_vertex_attribute.h>
#include <graphics/st_vertex_format.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>

st_gl_geometry::st_gl_geometry(
	const st_vertex_format* format,
	void* vertex_data,
	uint32_t vertex_size,
	uint32_t vertex_count,
	uint16_t* index_data,
	uint32_t index_count)
{
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(2, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * vertex_size, vertex_data, GL_STATIC_DRAW);

	size_t offset = 0;
	for (uint32_t itr = 0; itr < format->get_attribute_count(); ++itr)
	{
		const st_vertex_attribute* attr = &format->get_attribute(itr);
		GLint size = 0;
		size_t data_size = sizeof(float);
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;

		switch (attr->_type)
		{
		case st_vertex_attribute_position:
		case st_vertex_attribute_normal:
		case st_vertex_attribute_binormal:
		case st_vertex_attribute_tangent:
			size = 3;
			break;
		case st_vertex_attribute_joints:
			type = GL_UNSIGNED_INT;
			data_size = sizeof(uint32_t);
		case st_vertex_attribute_color:
		case st_vertex_attribute_weights:
			size = 4;
			break;
		case st_vertex_attribute_uv:
			size = 2;
			break;
		default:
			// TODO: Assert.
			break;
		}

		// HACK: We need the special case for joint indices.
		if (attr->_type == st_vertex_attribute_joints)
		{
			glVertexAttribIPointer(
				attr->_unit,
				size,
				type,
				vertex_size,
				(GLvoid*)offset);
		}
		else
		{
			glVertexAttribPointer(
				attr->_unit,
				size,
				type,
				normalized,
				vertex_size,
				(GLvoid*)offset);
		}

		glEnableVertexAttribArray(attr->_unit);

		offset += size * data_size;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(index_count * sizeof(uint32_t)), index_data, GL_STATIC_DRAW);

	_index_count = index_count;

	glBindVertexArray(0);
}

st_gl_geometry::~st_gl_geometry()
{
	glDeleteBuffers(2, _vbos);
	glDeleteVertexArrays(1, &_vao);
}

void st_gl_geometry::draw(st_gl_static_drawcall& draw_call)
{
	draw_call._vao = _vao;
	draw_call._index_count = _index_count;
	draw_call._draw_mode = st_primitive_topology_triangles;
}
