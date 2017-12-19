#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "entity/st_component.h"

#define DEBUG_DRAW_SKELETON 0

/*
** Component which drives animation; updates skeleton and skinning matrices.
*/
class st_animation_component : public st_component
{
public:
	st_animation_component(class st_entity* ent, struct st_model_data* model);
	virtual ~st_animation_component();

	virtual void update(struct st_frame_params* params) override;

	void play(struct st_animation* animation);

private:
	struct st_skeleton* _skeleton = 0;
	struct st_animation_playback* _playing = 0;
};
