#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <string>

/*
** A class that manages a scoped render group, for profiling.
**
** The intended usage is to create the marker on the stack at the top
** of the scope being profiled.
**
**		st_render_marker marker("profiling scope");
**
** The marker will clean itself up when it goes out of scope.
*/
class st_render_marker
{
public:
	st_render_marker(const std::string& message);
	~st_render_marker();
};
