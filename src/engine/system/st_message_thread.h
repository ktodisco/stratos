#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <Windows.h>

class st_input;
class st_window;

class st_message_thread
{
public:

	st_message_thread(st_window* window, st_input* input);
	~st_message_thread();

	bool pump_messages();
	LRESULT handle_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

private:

	st_window* _window;
	st_input* _input;
};