#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <string>
#include <Windows.h>

class st_message_thread;
class st_input;

class st_window
{
public:

	st_window(std::string window_name, uint32_t width, uint32_t height, st_input* input);
	~st_window();

	HWND get_window_handle() const
	{
		return _window_handle;
	}

	uint32_t get_width() const
	{
		return _width;
	}

	uint32_t get_height() const
	{
		return _height;
	}

	bool update();
	void close();
	void destroy() const;
	void show() const;

private:
	
	static LRESULT WINAPI static_wnd_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);
	LRESULT WINAPI wnd_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

private:

	uint32_t _width = 0;
	uint32_t _height = 0;
	HWND _window_handle = 0;
	HMODULE _module_handle = 0;
	st_message_thread* _message_thread = 0;

	bool _shutdown = false;
};
