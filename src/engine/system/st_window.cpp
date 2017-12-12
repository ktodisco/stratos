/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <system/st_window.h>

#include <system/st_message_thread.h>

st_window::st_window(std::string window_name, uint32_t width, uint32_t height, st_input* input) :
	_width(width), _height(height) 
{
	_module_handle = GetModuleHandle(NULL);

	WNDCLASSEX windowClass =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		static_wnd_proc,
		0L,
		0L,
		_module_handle,
		NULL,
		NULL,
		NULL,
		NULL,
		(LPCSTR)window_name.c_str(),
		NULL
	};

	RegisterClassEx(&windowClass);

	_window_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		(LPCSTR)window_name.c_str(),
		(LPCSTR)window_name.c_str(),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		(int)_width,
		(int)_height,
		GetDesktopWindow(),
		NULL,
		_module_handle,
		this);

	_message_thread = new st_message_thread(this, input);
}

st_window::~st_window()
{
	_window_handle = 0;
	_module_handle = 0;
}

bool st_window::update()
{
	return _message_thread->pump_messages() && !_shutdown;
}

void st_window::close()
{
	_shutdown = true;
}

void st_window::destroy() const
{
	DestroyWindow(_window_handle);
}

void st_window::show() const
{
	ShowWindow(_window_handle, SW_SHOWDEFAULT);
}

LRESULT WINAPI st_window::static_wnd_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    if (message == WM_NCCREATE)
    {
        SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)l_param)->lpCreateParams);
        return TRUE;
    }

	st_window* me = (st_window*)GetWindowLongPtr(window_handle, GWLP_USERDATA);

	LRESULT result = S_OK;
	if (me)
	{
		result = me->wnd_proc(window_handle, message, w_param, l_param);
	}

	return result;
}

LRESULT WINAPI st_window::wnd_proc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
	return _message_thread->handle_message(window_handle, message, w_param, l_param);
}
