#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_widget.h"

/*
** GUI text display widget.
*/
class st_label : public st_widget
{
public:
	st_label(const char* text, float x, float y, struct st_frame_params* params);
	~st_label();
};
