/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_input.h>

#include <framework/st_compiler_defines.h>
#include <framework/st_frame_params.h>

#include <cassert>
#include <cstdio>

#if defined(ST_MINGW)
#include <unistd.h>
#else
#include <thread>
#endif

#include <Windows.h>
#include <WinUser.h>

st_input::st_input() : _paused(false)
{
	_last_time = std::chrono::high_resolution_clock::now();
}

st_input::~st_input()
{
}

void st_input::handle_mouse_move(uint32_t mouse_x, uint32_t mouse_y)
{
	_mouse_x = mouse_x;
	_mouse_y = mouse_y;
}

void st_input::handle_key_press(int key_code, int info)
{
	switch (key_code)
	{
	case VK_LEFT: _button_mask |= k_button_left; break;
	case VK_RIGHT: _button_mask |= k_button_right; break;
	case VK_UP: _button_mask |= k_button_up; break;
	case VK_DOWN: _button_mask |= k_button_down; break;
	case VK_SPACE: _button_mask |= k_button_space; break;
	case 0x41: _button_mask |= k_button_a; break;
	case 0x42: _button_mask |= k_button_b; break;
	case 0x43: _button_mask |= k_button_c; break;
	case 0x44: _button_mask |= k_button_d; break;
	case 0x45: _button_mask |= k_button_e; break;
	case 0x46: _button_mask |= k_button_f; break;
	case 0x47: _button_mask |= k_button_g; break;
	case 0x48: _button_mask |= k_button_h; break;
	case 0x49: _button_mask |= k_button_i; break;
	case 0x4A: _button_mask |= k_button_j; break;
	case 0x4B: _button_mask |= k_button_k; break;
	case 0x4C: _button_mask |= k_button_l; break;
	case 0x4D: _button_mask |= k_button_m; break;
	case 0x4E: _button_mask |= k_button_n; break;
	case 0x4F: _button_mask |= k_button_o; break;
	case 0x50: _button_mask |= k_button_p; break;
	case 0x51: _button_mask |= k_button_q; break;
	case 0x52: _button_mask |= k_button_r; break;
	case 0x53: _button_mask |= k_button_s; break;
	case 0x54: _button_mask |= k_button_t; break;
	case 0x55: _button_mask |= k_button_u; break;
	case 0x56: _button_mask |= k_button_v; break;
	case 0x57: _button_mask |= k_button_w; break;
	case 0x58: _button_mask |= k_button_x; break;
	case 0x59: _button_mask |= k_button_y; break;
	case 0x5A: _button_mask |= k_button_z; break;
	default: break;
	};
}

void st_input::handle_key_release(int key_code, int info)
{
	switch (key_code)
	{
	case VK_LEFT: _button_mask &= ~k_button_left; break;
	case VK_RIGHT: _button_mask &= ~k_button_right; break;
	case VK_UP: _button_mask &= ~k_button_up; break;
	case VK_DOWN: _button_mask &= ~k_button_down; break;
	case VK_SPACE: _button_mask &= ~k_button_space; break;
	case 0x41: _button_mask &= ~k_button_a; break;
	case 0x42: _button_mask &= ~k_button_b; break;
	case 0x43: _button_mask &= ~k_button_c; break;
	case 0x44: _button_mask &= ~k_button_d; break;
	case 0x45: _button_mask &= ~k_button_e; break;
	case 0x46: _button_mask &= ~k_button_f; break;
	case 0x47: _button_mask &= ~k_button_g; break;
	case 0x48: _button_mask &= ~k_button_h; break;
	case 0x49: _button_mask &= ~k_button_i; break;
	case 0x4A: _button_mask &= ~k_button_j; break;
	case 0x4B: _button_mask &= ~k_button_k; break;
	case 0x4C: _button_mask &= ~k_button_l; break;
	case 0x4D: _button_mask &= ~k_button_m; break;
	case 0x4E: _button_mask &= ~k_button_n; break;
	case 0x4F: _button_mask &= ~k_button_o; break;
	case 0x50: _button_mask &= ~k_button_p; break;
	case 0x51: _button_mask &= ~k_button_q; break;
	case 0x52: _button_mask &= ~k_button_r; break;
	case 0x53: _button_mask &= ~k_button_s; break;
	case 0x54: _button_mask &= ~k_button_t; break;
	case 0x55: _button_mask &= ~k_button_u; break;
	case 0x56: _button_mask &= ~k_button_v; break;
	case 0x57: _button_mask &= ~k_button_w; break;
	case 0x58: _button_mask &= ~k_button_x; break;
	case 0x59: _button_mask &= ~k_button_y; break;
	case 0x5A: _button_mask &= ~k_button_z; break;
	default: break;
	}
}

void st_input::update(st_frame_params* params)
{
	params->_mouse_click_mask = 0;

	_pressed_mask = _button_mask & ~_previous_button_mask;
	_released_mask = _previous_button_mask & ~_button_mask;

	// Save the previous frame's button mask to compute released keys.
	_previous_button_mask = _button_mask;

	params->_button_mask = _button_mask;
	params->_mouse_press_mask = _mouse_button_mask;
	params->_mouse_x = _mouse_x;
	params->_mouse_y = _mouse_y;

	// Toggle pause if the p key is pressed.
	if (_pressed_mask & k_button_p)
	{
		_paused = !_paused;
	}

	// Update time. Cap frame rate at ~60 fps.
	auto t0 = _last_time;
	auto t1 = std::chrono::high_resolution_clock::now();

	auto delta_time = t1 - t0;
	auto min_delta_time = std::chrono::milliseconds(16);
	auto max_delta_time = std::chrono::milliseconds(32);
	if (delta_time < min_delta_time)
	{
#if defined(ST_MINGW) && defined(ST_32_BIT)
		uint32_t ms = std::chrono::duration_cast<std::chrono::duration<float>>(min_delta_time - delta_time).count();
		usleep(ms * 1000);
#else
		std::this_thread::sleep_for(min_delta_time - delta_time);
#endif
	}

	t1 = std::chrono::high_resolution_clock::now();
	_last_time = t1;

	params->_current_time = t1;
	params->_delta_time = t1 - t0;
	
	if (params->_delta_time > max_delta_time)
	{
		params->_delta_time = max_delta_time;
	}

	// Zero delta time if the sim is paused.
	if (_paused)
	{
		params->_delta_time = std::chrono::steady_clock::duration::zero();
	}

	// Allow for a single frame step.
	if (_pressed_mask & k_button_n)
	{
		params->_delta_time = min_delta_time;
		params->_single_step = true;
	}
}
