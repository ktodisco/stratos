/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_widget.h>

#include <framework/st_frame_params.h>

#include <graphics/st_drawcall.h>

const st_vec3f st_widget::k_button_color = { 0.25f, 0.25f, 1.0f };
const st_vec3f st_widget::k_button_hover_color = { 0.75f, 0.75f, 1.0f };
const st_vec3f st_widget::k_button_press_color = { 0.25f, 0.25f, 1.0f };
const st_vec3f st_widget::k_text_color = { 1.0f, 1.0f, 1.0f };
const float st_widget::k_button_offset = 4.0f;
const float st_widget::k_checkbox_offset = 15.0f;

st_widget::st_widget()
{
}

st_widget::~st_widget()
{
	_materials.clear();
}

void st_widget::draw_outline(st_frame_params* params, const st_vec2f& min, const st_vec2f& max, const st_vec3f& color, float offset)
{
	st_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ min.x - offset, min.y - offset, 0.0f });
	drawcall._positions.push_back({ max.x + offset, min.y - offset, 0.0f });
	drawcall._positions.push_back({ max.x + offset, max.y + offset, 0.0f });
	drawcall._positions.push_back({ min.x - offset, max.y + offset, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(0);

	drawcall._color = color;
	drawcall._draw_mode = st_primitive_topology_lines;
	drawcall._transform.make_identity();

	_materials.push_back(std::make_unique<st_constant_color_material>());
	_materials[_materials.size() - 1]->set_color(color);
	drawcall._material = _materials[_materials.size() - 1].get();

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}

void st_widget::draw_check(st_frame_params* params, const st_vec2f& min, const st_vec2f& max, const st_vec3f& color)
{
	st_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ min.x, min.y, 0.0f });
	drawcall._positions.push_back({ max.x, min.y, 0.0f });
	drawcall._positions.push_back({ max.x, max.y, 0.0f });
	drawcall._positions.push_back({ min.x, max.y, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(1);

	drawcall._color = color;
	drawcall._draw_mode = st_primitive_topology_lines;
	drawcall._transform.make_identity();

	_materials.push_back(std::make_unique<st_constant_color_material>());
	_materials[_materials.size() - 1]->set_color(color);
	drawcall._material = _materials[_materials.size() - 1].get();

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}

void st_widget::draw_fill(st_frame_params* params, const st_vec2f& min, const st_vec2f& max, const st_vec3f& color)
{
	st_dynamic_drawcall drawcall;

	drawcall._positions.push_back({ min.x, min.y, 0.0f });
	drawcall._positions.push_back({ max.x, min.y, 0.0f });
	drawcall._positions.push_back({ max.x, max.y, 0.0f });
	drawcall._positions.push_back({ min.x, max.y, 0.0f });

	drawcall._indices.push_back(0);
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(1);
	drawcall._indices.push_back(0);
	drawcall._indices.push_back(3);
	drawcall._indices.push_back(2);

	drawcall._color = color;
	drawcall._draw_mode = st_primitive_topology_triangles;
	drawcall._transform.make_identity();

	_materials.push_back(std::make_unique<st_constant_color_material>());
	_materials[_materials.size() - 1]->set_color(color);
	drawcall._material = _materials[_materials.size() - 1].get();

	while (params->_gui_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_gui_drawcalls.push_back(drawcall);
	params->_gui_drawcall_lock.clear(std::memory_order_release);
}
