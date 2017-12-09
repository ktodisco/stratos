#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <vector>

/*
** Represents the simulation stage of the frame.
** Owns the entities.
*/
class st_sim
{
public:
	st_sim();
	~st_sim();

	void add_entity(class st_entity* ent);

	void update(struct st_frame_params* params);
	void late_update(struct st_frame_params* params);

private:
	std::vector<class st_entity*> _entities;
};
