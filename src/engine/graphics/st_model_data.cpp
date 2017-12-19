/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_model_data.h"

#include "st_animation.h"

st_model_data::st_model_data()
{
}

st_model_data::~st_model_data()
{
	if (_skeleton)
	{
		delete _skeleton;
	}
}