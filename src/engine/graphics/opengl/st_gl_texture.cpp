/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/opengl/st_gl_texture.h>

#include <cassert>
#include <stb_image.h>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

st_gl_texture::st_gl_texture()
{
	glGenTextures(1, &_handle);
}

st_gl_texture::~st_gl_texture()
{
	glDeleteTextures(1, &_handle);
}

void st_gl_texture::load_from_data(uint32_t width, uint32_t height, e_st_texture_format format, void* data)
{
	GLenum internal_format;

	switch (format)
	{
	case st_texture_format_r8_uint:
		internal_format = GL_R8;
		break;
	case st_texture_format_r8g8b8a8_uint:
		internal_format = GL_RGBA8;
		break;
	default:
		assert(false);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
}

bool st_gl_texture::load_from_file(const char* path)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	int width, height, channels_in_file;
	uint8_t* data = stbi_load(fullpath.c_str(), &width, &height, &channels_in_file, 4);
	if (!data)
	{
		return false;
	}

	load_from_data(width, height, st_texture_format_r8g8b8a8_uint, data);

	stbi_image_free(data);

	return true;
}
