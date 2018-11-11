#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_component.h>

#include <math/st_vec3f.h>

#include <memory>

class st_light_component : public st_component
{
public:
	st_light_component(
		class st_entity* ent,
		st_vec3f color,
		float power);
	virtual ~st_light_component();

	virtual void update(struct st_frame_params* params) override;

private:
	std::unique_ptr<class st_sphere_light> _light = nullptr;
};
