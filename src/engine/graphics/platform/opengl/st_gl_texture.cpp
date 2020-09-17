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
#include <string>

st_gl_texture::st_gl_texture(
	uint32_t width,
	uint32_t height,
	uint32_t levels,
	e_st_format format,
	e_st_texture_usage_flags usage,
	e_st_texture_state initial_state,
	void* data) :
	_width(width), _height(height), _levels(levels), _format(format)
{
	glGenTextures(1, &_handle);

	glBindTexture(GL_TEXTURE_2D, _handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexStorage2D(GL_TEXTURE_2D, levels, format, width, height);

	if (data)
	{
		if (is_compressed(format))
		{
			uint8_t* bits = reinterpret_cast<uint8_t*>(data);

			for (uint32_t mip = 0; mip < levels; ++mip)
			{
				uint32_t level_width = width >> mip;
				uint32_t level_height = height >> mip;

				size_t row_bytes;
				size_t num_rows;
				get_surface_info(level_width, level_height, format, nullptr, &row_bytes, &num_rows);

				glCompressedTexSubImage2D(
					GL_TEXTURE_2D,
					mip,
					0,
					0,
					level_width,
					level_height,
					format,
					row_bytes * num_rows,
					bits);

				bits += row_bytes * num_rows;
			}
		}
		else
		{
			GLenum pixel_format;
			GLenum type;
			get_pixel_format_and_type(format, pixel_format, type);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, type, data);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

st_gl_texture::~st_gl_texture()
{
	glDeleteTextures(1, &_handle);
}

void st_gl_texture::set_meta(const char* name)
{
	_name = name;
}

void st_gl_texture::set_name(std::string name)
{
	glObjectLabel(GL_TEXTURE, _handle, name.length(), name.c_str());
}

void st_gl_texture::bind(class st_gl_render_context* context, GLuint sampler)
{
	const st_gl_shader* shader = context->get_bound_shader();

	st_gl_uniform uniform = shader->get_uniform(_name.c_str());
	uniform.set(*this, uniform.get_location());

	glBindSampler(uniform.get_location(), sampler);
}

void st_gl_texture::get_pixel_format_and_type(
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

#endif
