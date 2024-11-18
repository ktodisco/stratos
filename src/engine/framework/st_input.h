#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <chrono>
#include <cstdint>

/*
** Keyboard/controller buttons we track.
*/
enum st_button_t
{
	k_button_left = 1 << 0,
	k_button_right = 1 << 1,
	k_button_up = 1 << 2,
	k_button_down = 1 << 3,
	k_button_space = 1 << 4,
	k_button_a = 1 << 5,
	k_button_b = 1 << 6,
	k_button_c = 1 << 7,
	k_button_d = 1 << 8,
	k_button_e = 1 << 9,
	k_button_f = 1 << 10,
	k_button_g = 1 << 11,
	k_button_h = 1 << 12,
	k_button_i = 1 << 13,
	k_button_j = 1 << 14,
	k_button_k = 1 << 15,
	k_button_l = 1 << 16,
	k_button_m = 1 << 17,
	k_button_n = 1 << 18,
	k_button_o = 1 << 19,
	k_button_p = 1 << 20,
	k_button_q = 1 << 21,
	k_button_r = 1 << 22,
	k_button_s = 1 << 23,
	k_button_t = 1 << 24,
	k_button_u = 1 << 25,
	k_button_v = 1 << 26,
	k_button_w = 1 << 27,
	k_button_x = 1 << 28,
	k_button_y = 1 << 29,
	k_button_z = 1 << 30,
	k_button_shift = 1 << 31,
};

/*
** Mouse interactions we track.
*/
enum k_mouse_t
{
	k_mouse_left = 1 << 0,
	k_mouse_right = 1 << 1,
};

/*
** Represents the input stage of the frame.
** Owns the window, user input devices and clock.
*/
class st_input
{
public:
	st_input();
	~st_input();

	void update(struct st_frame_params* params);
	void handle_mouse_press(int32_t key_code, int32_t info);
	void handle_mouse_release(int32_t key_code, int32_t info);
	void handle_mouse_move(uint32_t mouse_x, uint32_t mouse_y);
	void handle_key_press(int32_t key_code, int32_t info);
	void handle_key_release(int32_t key_code, int32_t info);

private:
	uint64_t _button_mask = 0;
	uint64_t _previous_button_mask = 0;
	uint64_t _pressed_mask = 0;
	uint64_t _released_mask = 0;

	uint64_t _mouse_press_mask = 0;
	uint64_t _mouse_click_mask = 0;

	float _mouse_x = 0.0f;
	float _mouse_y = 0.0f;
	float _mouse_delta_x = 0.0f;
	float _mouse_delta_y = 0.0f;

	std::chrono::high_resolution_clock::time_point _last_time;

	bool _paused;
};
