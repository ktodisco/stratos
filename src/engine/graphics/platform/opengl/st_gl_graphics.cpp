/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_graphics.h>

#include <cassert>
#include <iostream>

void gl_message_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		fprintf( stderr,
			"GL ERROR: type = 0x%x, severity = 0x%x, message = %s\n",
			type,
			severity,
			message );
		assert(false);
	}
}

void get_pixel_format_and_type(
	e_st_format format,
	GLenum& pixel_format,
	GLenum& type)
{
	switch (format)
	{
	case st_format_r8_unorm:
		pixel_format = GL_RED;
		type = GL_UNSIGNED_BYTE;
		break;
	case st_format_r8g8b8a8_snorm:
	case st_format_r8g8b8a8_unorm:
		pixel_format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case st_format_r16g16b16a16_float:
	case st_format_r32g32b32a32_float:
		pixel_format = GL_RGBA;
		type = GL_FLOAT;
		break;
	case st_format_d24_unorm_s8_uint:
		pixel_format = GL_DEPTH_STENCIL;
		type = GL_UNSIGNED_INT_24_8;
		break;
	default:
		assert(false);
		break;
	}
}
