/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_texture.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <graphics/platform/opengl/st_gl_render_context.h>
#include <graphics/st_shader.h>

#include <cassert>
#include <stb_image.h>
#include <string>

st_gl_texture::st_gl_texture()
{
	glGenTextures(1, &_handle);
}

st_gl_texture::~st_gl_texture()
{
	glDeleteTextures(1, &_handle);
}

void st_gl_texture::reserve_data(uint32_t width, uint32_t height, e_st_texture_format format)
{
	_format = format;

	GLenum pixel_format;
	GLenum type;
	get_pixel_format_and_type(format, pixel_format, type);

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, pixel_format, type, 0);
}

void st_gl_texture::load_from_data(uint32_t width, uint32_t height, e_st_texture_format format, void* data)
{
	_format = format;

	GLenum pixel_format;
	GLenum type;
	get_pixel_format_and_type(format, pixel_format, type);

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, type, data);
}

bool st_gl_texture::load_from_file(const char* path)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += (path && path[0]) ? path : "data/textures/default_albedo.png";

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

void st_gl_texture::set_meta(const char* name, const uint32_t slot)
{
	_name = name;
	_slot = slot;
}

void st_gl_texture::set_name(std::string name)
{
	glObjectLabel(GL_TEXTURE, _handle, name.length(), name.c_str());
}

void st_gl_texture::bind(class st_gl_render_context* context)
{
	const st_gl_shader* shader = context->get_bound_shader();

	st_gl_uniform uniform = shader->get_uniform(_name.c_str());
	uniform.set(*this, _slot);
}

void st_gl_texture::get_pixel_format_and_type(
	e_st_texture_format format,
	GLenum& pixel_format,
	GLenum& type)
{
	switch (format)
	{
	case st_texture_format_r8_unorm:
		pixel_format = GL_RED;
		type = GL_UNSIGNED_BYTE;
		break;
	case st_texture_format_r8g8b8a8_snorm:
	case st_texture_format_r8g8b8a8_unorm:
		pixel_format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;
	case st_texture_format_r16g16b16a16_float:
		pixel_format = GL_RGBA;
		type = GL_FLOAT;
		break;
	case st_texture_format_d24_unorm_s8_uint:
		pixel_format = GL_DEPTH_STENCIL;
		type = GL_UNSIGNED_INT_24_8;
		break;
	default:
		assert(false);
		break;
	}
}

#endif
