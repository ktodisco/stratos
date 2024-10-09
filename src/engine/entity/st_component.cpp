/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_component.h>

#include <entity/st_entity.h>

st_component::st_component(st_entity* ent) : _entity(ent)
{
}

st_component::~st_component()
{
}

void st_component::update(st_frame_params* params)
{
}

void st_component::late_update(st_frame_params* params)
{
}

void st_component::debug()
{
}
