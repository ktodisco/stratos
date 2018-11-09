/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <system/st_message_thread.h>

#include <framework/st_input.h>

#include <system/st_window.h>

#include <windowsx.h>

st_message_thread::st_message_thread(st_window* window, st_input* input) :
	_window(window), _input(input)
{
}

st_message_thread::~st_message_thread()
{
}

bool st_message_thread::pump_messages()
{
	MSG message;

	if (PeekMessage(&message, _window->get_window_handle(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return true;
}

LRESULT st_message_thread::handle_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch(message)
	{
	case WM_MOUSEMOVE:
		{
			uint32_t x_pos = GET_X_LPARAM(l_param);
			uint32_t y_pos = GET_Y_LPARAM(l_param);
			_input->handle_mouse_move(x_pos, y_pos);
		}
		break;
	case WM_KEYDOWN:
		_input->handle_key_press((int)w_param, (int)l_param);
		break;
	case WM_KEYUP:
		_input->handle_key_release((int)w_param, (int)l_param);
		break;
	case WM_CREATE:
		break;
	case WM_CLOSE:
		_window->close();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		ValidateRect(window_handle, NULL);
		break;
	case WM_SIZE:
		break;
	}

	return DefWindowProc(window_handle, message, w_param, l_param);
}