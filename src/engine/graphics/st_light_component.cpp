/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_light_component.h>

#include <entity/st_entity.h>

#include <graphics/st_point_light.h>

st_light_component::st_light_component(
	st_entity* ent,
	st_vec3f color,
	float power) :
	st_component(ent)
{
	_light = std::make_unique<st_point_light>(
		ent->get_transform().get_translation(),
		color,
		power);
}

st_light_component::~st_light_component()
{
}

void st_light_component::update(struct st_frame_params* params)
{
	_light->set_position(get_entity()->get_transform().get_translation());
	params->_light = _light.get();
}
