/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_font.h>

#include <framework/st_compiler_defines.h>
#include <framework/st_frame_params.h>
#include <framework/st_global_resources.h>
#include <framework/st_output.h>

#include <graphics/geometry/st_vertex_attribute.h>
#include <graphics/material/st_material.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>

#include <math/st_vec2f.h>

#include <algorithm>
#include <iostream>

#if defined(ST_MINGW)
#include <stdio.h>
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
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

	st_texture_desc desc;
	desc._width = image_width;
	desc._height = image_height;
	desc._levels = 1;
	desc._format = st_format_r8_unorm;
	desc._usage = e_st_texture_usage::sampled;
	desc._initial_state = st_texture_state_pixel_shader_read;
	desc._data = image_data;
	_texture = st_graphics_context::get()->create_texture(desc);
	delete[] image_data;

	_material = std::make_unique<st_font_material>(_texture.get());
}

st_font::~st_font()
{
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
	drawcall._material = _material.get();
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
				extent_min->x = std::min(extent_min->x, q.x0);
				extent_min->y = std::min(extent_min->y, q.y0);
			}
			if (extent_max)
			{
				extent_max->x = std::max(extent_max->x, q.x1);
				extent_max->y = std::max(extent_max->y, q.y1);
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
			drawcall._indices.push_back(index + 2);
			drawcall._indices.push_back(index + 1);
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

st_font_material::st_font_material(st_texture* texture) :
	st_material(e_st_render_pass_type::ui),
	_texture(texture)
{
	st_graphics_context* context = st_graphics_context::get();

	{
		st_buffer_desc desc;
		desc._count = 1;
		desc._element_size = sizeof(st_font_cb);
		desc._usage = e_st_buffer_usage::uniform;
		_constant_buffer = context->create_buffer(desc);
		context->add_constant(_constant_buffer.get(), "type_cb0", st_shader_constant_type_block);
	}

	std::vector<st_vertex_attribute> attributes;
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32b32_float, 0));
	attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r32g32b32_float, 1));
	_vertex_format = context->create_vertex_format(attributes.data(), attributes.size());

	st_output* output = st_output::get();

	st_pipeline_state_desc desc;
	desc._shader = st_shader_manager::get()->get_shader(st_shader_font);
	desc._depth_stencil_desc._depth_enable = false;
	desc._blend_desc._target_blend[0]._blend = true;
	desc._blend_desc._target_blend[0]._src_blend = st_blend_src_alpha;
	desc._blend_desc._target_blend[0]._src_blend_alpha = st_blend_src_alpha;
	desc._blend_desc._target_blend[0]._dst_blend = st_blend_inv_src_alpha;
	desc._blend_desc._target_blend[0]._dst_blend_alpha = st_blend_inv_src_alpha;
	desc._vertex_format = _vertex_format.get();
	desc._render_target_count = 1;
	desc._render_target_formats[0] = st_format_r8g8b8a8_unorm;
	desc._depth_stencil_format = st_format_d24_unorm_s8_uint;
	output->get_target_formats(e_st_render_pass_type::ui, desc);

	_pipeline = context->create_pipeline(desc);

	_resource_table = context->create_resource_table();
	st_buffer* cbs[] = { _constant_buffer.get() };
	context->set_constant_buffers(_resource_table.get(), 1, cbs);

	if (_texture)
	{
		st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
		context->set_textures(_resource_table.get(), 1, &_texture, samplers);
	}
}

st_font_material::~st_font_material()
{
	_pipeline = nullptr;
	_vertex_format = nullptr;
	_resource_table = nullptr;
}

void st_font_material::bind(
	st_graphics_context* context,
	e_st_render_pass_type pass_type,
	const st_frame_params* params,
	const st_mat4f& proj,
	const st_mat4f& view,
	const st_mat4f& transform)
{
	context->set_pipeline(_pipeline.get());

	st_mat4f mvp = transform * view * proj;

	st_font_cb cb_data{};
	cb_data._mvp = mvp;
	cb_data._color = _color;
	context->update_buffer(_constant_buffer.get(), &cb_data, 0, 1);

	context->bind_resource_table(_resource_table.get());
}
