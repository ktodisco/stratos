/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_rigid_body.h"
#include "st_shape.h"

st_rigid_body::st_rigid_body(st_shape* shape, float mass) : _mass(mass), _shape(shape), _flags(0)
{
	_transform.make_identity();
	_orientation.make_axis_angle(st_vec3f::y_vector(), 0);

	_shape->get_inertia_tensor(_inertia_tensor, _mass);
}

st_rigid_body::~st_rigid_body()
{
}

void st_rigid_body::get_debug_draw(st_procedural_drawcall* drawcall)
{
	_shape->get_debug_draw(_transform, drawcall);
}

void st_rigid_body::make_static()
{
	_flags |= k_static;
}

void st_rigid_body::make_weightless()
{
	_flags |= k_weightless;
}

void st_rigid_body::add_linear_velocity(const st_vec3f& v)
{
	_velocity += v;
}

void st_rigid_body::add_angular_momentum(const st_vec3f& v)
{
	_angular_momentum += v;
}
