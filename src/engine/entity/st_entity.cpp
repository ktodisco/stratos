/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_entity.h>

#include <entity/st_component.h>

st_entity::st_entity()
{
	_transform.make_identity();
}

st_entity::~st_entity()
{
}

void st_entity::add_component(st_component* comp)
{
	_components.push_back(comp);
}

void st_entity::update(st_frame_params* params)
{
	for (auto& c : _components)
	{
		c->update(params);
	}
}

void st_entity::late_update(st_frame_params* params)
{
	for (auto& c : _components)
	{
		c->late_update(params);
	}
}

void st_entity::translate(const st_vec3f& translation)
{
	_transform.translate(translation);
}

void st_entity::rotate(const st_quatf& rotation)
{
	st_mat4f rotation_m;
	rotation_m.make_rotation(rotation);
	_transform = rotation_m * _transform;
}

void st_entity::scale(float s)
{
	_transform.scale(s);
}
