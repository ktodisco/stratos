#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/st_component.h"

/*
** A component that adds physics simulation to an entity.
** Owns a rigid body and synchronizes its transform and that of the entity.
*/
class st_physics_component : public st_component
{
public:
	st_physics_component(class st_entity* ent, struct st_shape* shape, float mass);
	virtual ~st_physics_component();

	virtual void update(struct st_frame_params* params) override;
	virtual void late_update(struct st_frame_params* params) override;

	class st_rigid_body* get_rigid_body() const { return _body; }

private:
	class st_rigid_body* _body;
};
