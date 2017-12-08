#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_component.h"

#include <string>

/*
** A test component that generates an empty draw call.
*/
class st_hello_component : public st_component
{
public:
	st_hello_component(class st_entity* ent, const char* name);
	virtual ~st_hello_component();

	virtual void update(struct st_frame_params* params) override;

private:
	std::string _name;
};
