/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_sun_component.h>

#include <math/st_math.h>

#include <imgui.h>

#include <cmath>

st_sun_component::st_sun_component(
	st_entity* ent,
	float azimuth,
	float angle,
	st_vec3f color,
	float power)
	: st_component(ent), _azimuth(azimuth), _angle(angle)
{
	_light = std::make_unique<st_directional_light>(calculate_direction(), color, power);
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
		bool change = ImGui::DragFloat("Azimuth", &_azimuth, 1.0f, 0.0f, 360.0f, "%.1f");
		change |= ImGui::DragFloat("Angle", &_angle, 1.0f, 0.0f, 90.0f, "%.1f");

		if (change)
			_light->_direction = calculate_direction();

		ImGui::ColorEdit3("Color", _light->_color.axes);
		ImGui::DragFloat("Power", &_light->_power, 1.0f, 0.0f, 1e5f, "%.0f");

		ImGui::TreePop();
	}
}

st_vec3f st_sun_component::calculate_direction() const
{
	// Given the azimuth and angle, calculate the direction of incoming sunlight.
	float azimuth_radians = st_degrees_to_radians(_azimuth);
	float angle_radians = st_degrees_to_radians(_angle);
	st_vec3f negative_dir =
	{
		cos(azimuth_radians) * sin(angle_radians),
		cos(angle_radians),
		sin(azimuth_radians) * sin(angle_radians),
	};

	negative_dir.normalize();

	return -negative_dir;
}