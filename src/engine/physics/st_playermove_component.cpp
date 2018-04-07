/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <physics/st_playermove_component.h>

#include <entity/st_entity.h>

st_playermove_component::st_playermove_component(st_entity* ent)
	: st_component(ent), _move_when_paused(false)
{
}

st_playermove_component::~st_playermove_component()
{
}

void st_playermove_component::update(st_frame_params* params)
{
	float dt = _move_when_paused ?
		1.0f / 60.0f :
		std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();

	if (params->_button_mask & k_button_j)
	{
		get_entity()->translate({ -5.0f * dt, 0.0f, 0.0f });
	}
	if (params->_button_mask & k_button_l)
	{
		get_entity()->translate({ 5.0f * dt, 0.0f, 0.0f });
	}
	if (params->_button_mask & k_button_i)
	{
		get_entity()->translate({ 0.0f, 5.0f * dt, 0.0f });
	}
	if (params->_button_mask & k_button_k)
	{
		get_entity()->translate({ 0.0f, -5.0f * dt, 0.0f });
	}
	if (params->_button_mask & k_button_o)
	{
		st_vec3f axis = { 1.0f, 1.0f, 1.0f };
		st_quatf axis_angle;
		axis_angle.make_axis_angle(axis, st_degrees_to_radians(180.0f * dt));
		get_entity()->rotate(axis_angle);
	}
}

void st_playermove_component::set_move_when_paused(bool state)
{
	_move_when_paused = state;
}
