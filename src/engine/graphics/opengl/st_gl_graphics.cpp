/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_graphics.h>

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
	}
}
