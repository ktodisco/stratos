#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/material/st_material.h"
#include "math/st_vec3f.h"

#include <memory>

#include <stb_truetype.h>

/*
** Represents a TrueType font of a certain pixel height.
** Capable of displaying ASCII characters from that font on screen.
*/
class st_font
{
public:
	st_font(const char* path, float char_height, int image_width, int image_height);
	~st_font();

	void print(
		struct st_frame_params* params,
		const char* text,
		float x,
		float y,
		const struct st_vec3f& color,
		struct st_vec2f* extent_min = nullptr,
		struct st_vec2f* extent_max = nullptr);

private:
	std::unique_ptr<class st_texture> _texture;
	std::unique_ptr<class st_font_material> _material;

	int _image_width;
	int _image_height;

	enum { k_character_start = 32, k_character_count = 96 };
	stbtt_bakedchar _characters[k_character_count];
};

/*
** Material used to render st_font.
*/
class st_font_material : public st_material
{
public:
	st_font_material(st_texture* texture);
	~st_font_material();

	virtual void bind(
		class st_graphics_context* context,
		const struct st_frame_params* params,
		const struct st_mat4f& proj,
		const struct st_mat4f& view,
		const struct st_mat4f& transform) override;

	void get_pipeline_state(
		struct st_pipeline_state_desc* state_desc) override;

	st_material_type get_material_type() override { return st_material_type_font; }

	void set_color(const struct st_vec3f& color) override { _color = color; }

	struct st_font_cb
	{
		st_mat4f _mvp;
		st_vec3f _color;
	};

private:
	std::unique_ptr<struct st_buffer> _constant_buffer = nullptr;
	class st_texture* _texture;
	st_vec3f _color;
	std::unique_ptr<struct st_resource_table> _resource_table = nullptr;
};
