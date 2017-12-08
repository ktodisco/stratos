/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_hello_component.h"

st_hello_component::st_hello_component(st_entity* ent, const char* name) : st_component(ent), _name(name)
{
}

st_hello_component::~st_hello_component()
{
}

void st_hello_component::update(st_frame_params* params)
{
}
