/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_physics_component.h"
#include "st_physics_world.h"
#include "st_rigid_body.h"

#include "entity/st_entity.h"

st_physics_component::st_physics_component(st_entity* ent, st_shape* shape, float mass)
	: st_component(ent)
{
	_body = new st_rigid_body(shape, mass);
	_body->_transform = ent->get_transform();
}

st_physics_component::~st_physics_component()
{
	delete _body;
}

void st_physics_component::update(st_frame_params* params)
{
	// First, re-sync the rigid body's transform with the entity's.
	_body->_transform = get_entity()->get_transform();

#if st_PHYSICS_DEBUG_DRAW
	st_dynamic_drawcall draw;
	_body->get_debug_draw(&draw);

	while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_dynamic_drawcalls.push_back(draw);
	params->_dynamic_drawcall_lock.clear(std::memory_order_release);
#endif
}

void st_physics_component::late_update(st_frame_params* params)
{
	// Sync the entity's transform with the rigid body's.
	get_entity()->set_transform(_body->_transform);
}
