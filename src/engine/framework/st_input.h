#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <chrono>
#include <cstdint>

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
	void handle_key_press(int key_code, int info);
	void handle_key_release(int key_code, int info);

private:
	uint64_t _button_mask = 0;
	uint64_t _previous_button_mask = 0;
	uint64_t _pressed_mask = 0;
	uint64_t _released_mask = 0;

	uint64_t _mouse_button_mask = 0;

	float _mouse_x = 0.0f;
	float _mouse_y = 0.0f;

	std::chrono::high_resolution_clock::time_point _last_time;

	bool _paused;
};
