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

#include <cstdint>

/*
** Renderable basic textured cubed.
*/
class st_cube_component : public st_component
{
public:
	st_cube_component(class st_entity* ent, const char* texture_file);
	virtual ~st_cube_component();

	virtual void update(struct st_frame_params* params) override;

private:
	class st_material* _material;
	uint32_t _vao;
	uint32_t _vbos[4];
	uint32_t _index_count;
};
