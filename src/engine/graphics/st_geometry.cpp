/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_geometry.h"

#include "st_animation.h"

st_model::st_model()
{
}

st_model::~st_model()
{
	if (_skeleton)
	{
		delete _skeleton;
	}
}