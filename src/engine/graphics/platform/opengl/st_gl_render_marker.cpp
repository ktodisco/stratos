/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_render_marker.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <string>

st_gl_render_marker::st_gl_render_marker(const std::string& message)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, message.length(), message.c_str());
}

st_gl_render_marker::~st_gl_render_marker()
{
	glPopDebugGroup();
}

#endif
