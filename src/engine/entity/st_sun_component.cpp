/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_sun_component.h>

#include <imgui.h>

st_sun_component::st_sun_component(
	st_entity* ent,
	float azimuth,
	float angle,
	st_vec3f color,
	float power)
	: st_component(ent), _azimuth(azimuth), _angle(angle)
{
	st_vec3f dir { 1.0f, -1.0f, 0.0f };
	dir.normalize();

	_light = std::make_unique<st_directional_light>(dir, color, power);
}

st_sun_component::~st_sun_component()
{
	_light = nullptr;
}

void st_sun_component::update(struct st_frame_params* params)
{
	params->_sun = _light.get();
}

void st_sun_component::debug()
{
	if (ImGui::TreeNode("st_sun_component"))
	{
		ImGui::DragFloat("Azimuth", &_azimuth, 1.0f, 0.0f, 360.0f, "%.1f");
		ImGui::DragFloat("Angle", &_angle, 1.0f, 0.0f, 90.0f, "%.1f");

		ImGui::ColorEdit3("Color", _light->_color.axes);
		ImGui::DragFloat("Power", &_light->_power, 1.0f, 0.0f, 1e5f, "%.0f");

		ImGui::TreePop();
	}
}
