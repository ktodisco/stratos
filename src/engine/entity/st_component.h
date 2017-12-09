#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/st_frame_params.h"

/*
** Base class component object.
** All entity functionality is expected to derive from this object.
** @see st_entity
*/
class st_component
{
public:
	st_component() = delete;
	st_component(class st_entity* ent);
	virtual ~st_component();

	virtual void update(struct st_frame_params* params);
	virtual void late_update(struct st_frame_params* params);

	const class st_entity* get_entity() const { return _entity; }
	class st_entity* get_entity() { return _entity; }

private:
	class st_entity* _entity;
};
