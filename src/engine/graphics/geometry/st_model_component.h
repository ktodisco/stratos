#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_component.h>

#include <cstdint>
#include <memory>

/*
** Renderable model component.
*/
class st_model_component : public st_component
{
public:
	st_model_component(class st_entity* entity, struct st_model_data* model, std::unique_ptr<class st_material> material);
	virtual ~st_model_component();

	virtual void update(struct st_frame_params* params) override;

private:
	std::unique_ptr<class st_material> _material = nullptr;
	std::unique_ptr<struct st_geometry> _geometry = nullptr;
};
