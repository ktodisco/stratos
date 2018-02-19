/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_font.h>

#include <framework/st_compiler_defines.h>
#include <framework/st_frame_params.h>
#include <graphics/st_material.h>
#include <graphics/st_render_context.h>
#include <math/st_vec2f.h>

#include <algorithm>
#include <iostream>

#if defined(ST_MINGW)
#include <stdio.h>
#endif

st_font::st_font(const char* path, float char_height, int image_width, int image_height)
	: _image_width(image_width), _image_height(image_height)
{
	extern char g_root_path[256];
	std::string fullpath = g_root_path;
	fullpath += path;

	FILE* font_file = fopen(fullpath.c_str(), "rb");
	if (!font_file)
	{
		std::cerr << "Failed to open font: " << fullpath << std::endl;
		return;
	}

	fseek(font_file, 0, SEEK_END);
	size_t font_size = ftell(font_file);
	fseek(font_file, 0, SEEK_SET);

	unsigned char* font_buffer = new unsigned char[font_size];
	if (fread(font_buffer, 1, font_size, font_file) != font_size)
	{
		std::cerr << "Failed to read font: " << fullpath << std::endl;
		delete[] font_buffer;
		return;
	}

	unsigned char* image_data = new unsigned char[image_width * image_height];

	int result = stbtt_BakeFontBitmap(
		font_buffer, 0, char_height,
		image_data, image_width, image_height,
		k_character_start, k_character_count, _characters);

	delete[] font_buffer;

	if (result < 0)
	{
		std::cerr << "Failed to bake font: " << fullpath << std::endl;
		delete[] image_data;
		return;
	}

	_texture = new st_texture();
	_texture->load_from_data(image_width, image_height, st_texture_format_r8_uint, image_data);
	delete[] image_data;

	_material = new st_font_material(_texture);
	//_material->init();
}

st_font::~st_font()
{
	delete _material;
	delete _texture;
}

void st_font::print(
	st_frame_params* params,
	const char* text,
	float x,
	float y,
	const st_vec3f& color,
	st_vec2f* extent_min,
	st_vec2f* extent_max)
{
	if (extent_min)
	{
		*extent_min = { x, y };
	}
	if (extent_max)
	{
		*extent_max = { x, y };
	}

	st_dynamic_drawcall drawcall;
	drawcall._color = color;
	drawcall._draw_mode = st_primitive_topology_triangles;
	drawcall._material = _material;
	drawcall._transform.make_identity();

	int index = 0;
	while (*text)
	{
		if (*text >= k_character_start && *text < k_character_start + k_character_count)
		{
			stbtt_aligned_quad q;
			stbtt_GetBakedQuad(_characters, _image_width, _image_height, *text - k_character_start, &x, &y, &q, 1);

			if (extent_min)
			{
				extent_min->x = min(extent_min->x, q.x0);
				extent_min->y = min(extent_min->y, q.y0);
			}
			if (extent_max)
			{
				extent_max->x = max(extent_max->x, q.x1);
				extent_max->y = max(extent_max->y, q.y1);
			}

			drawcall._positions.push_back({ q.x0, q.y0, 0.0f });
			drawcall._positions.push_back({ q.x1, q.y0, 0.0f });
			drawcall._positions.push_back({ q.x1, q.y1, 0.0f });
			drawcall._positions.push_back({ q.x0, q.y1, 0.0f });

			drawcall._texcoords.push_back({ q.s0, q.t0 });
			drawcall._texcoords.push_back({ q.s1, q.t0 });
			drawcall._texcoords.push_back({ q.s1, q.t1 });
			drawcall._texcoords.push_back({ q.s0, q.t1 });

			drawcall._indices.push_back(index + 0);
			drawcall._indices.push_back(index + 1);
			drawcall._indices.push_back(index + 2);
			drawcall._indices.push_back(index + 0);
			drawcall._indices.push_back(index + 3);
			drawcall._indices.push_back(index + 2);
			index += 4;
		}
		++text;
	}

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}

st_font_material::st_font_material(st_texture* texture) : _texture(texture)
{
}

st_font_material::~st_font_material()
{
}

/*bool st_font_material::init()
{
	return st_material::init_shaders(
		"data/shaders/st_font_simple");
}*/

void st_font_material::bind(
	st_render_context* context,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	/*st_uniform mvp_uniform = _program->get_uniform("u_mvp");
	st_uniform color_uniform = _program->get_uniform("u_color");
	st_uniform texture_uniform = _program->get_uniform("u_texture");

	_program->use();

	mvp_uniform.set(transform * view * proj);
	color_uniform.set(_color);
	texture_uniform.set(*_texture, 0);

	context->set_blend_state(true, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(true, k_st_depth_less);
	context->set_depth_mask(true);

	context->set_blend_state(true, k_st_src_alpha, k_st_one_minus_src_alpha);
	context->set_depth_state(false, 0);
	context->set_depth_mask(false);*/
}
