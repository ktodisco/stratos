#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/st_component.h"

/*
** A component that drives entity transform through user input.
*/
class st_playermove_component : public st_component
{
public:
	st_playermove_component(class st_entity* ent);
	virtual ~st_playermove_component();

	virtual void update(struct st_frame_params* params) override;

	void set_move_when_paused(bool state);

private:
	bool _move_when_paused;
};
