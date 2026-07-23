#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/st_graphics.h>

#include <string>

class st_render_marker
{
public:

	st_render_marker(st_command_list* command_list, const std::string& marker) :
		_command_list(command_list)
	{
		_command_list->begin_marker(marker);
	}

	~st_render_marker()
	{
		_command_list->end_marker();
	}

private:
	st_command_list* _command_list;
};
