/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_render_marker.h"

#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

st_render_marker::st_render_marker(const std::string& message)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, message.length(), message.c_str());
}

st_render_marker::~st_render_marker()
{
	glPopDebugGroup();
}
