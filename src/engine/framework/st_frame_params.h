#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "graphics/st_drawcall.h"
#include "math/st_mat4f.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <vector>

/*
** Working information for the frame.
** Each frame stage emits some data for consumption by later stages.
*/
struct st_frame_params
{
	// Data emitted by input stage:
	std::chrono::high_resolution_clock::time_point _current_time;
	std::chrono::high_resolution_clock::duration _delta_time;

	uint64_t _button_mask;

	uint64_t _mouse_click_mask;
	uint64_t _mouse_press_mask;
	float _mouse_x;
	float _mouse_y;
	float _mouse_delta_x;
	float _mouse_delta_y;

	// Data emitted by sim stage:
	std::vector<st_static_drawcall> _static_drawcalls;
	std::atomic_flag _static_drawcall_lock = ATOMIC_FLAG_INIT;

	std::vector<st_dynamic_drawcall> _dynamic_drawcalls;
	std::atomic_flag _dynamic_drawcall_lock = ATOMIC_FLAG_INIT;

	std::vector<st_dynamic_drawcall> _gui_drawcalls;
	std::atomic_flag _gui_drawcall_lock = ATOMIC_FLAG_INIT;

	st_mat4f _view;
	st_mat4f _projection;
	st_vec3f _eye;
	uint32_t _width;
	uint32_t _height;

	// Sun.
	struct st_directional_light* _sun = nullptr;
	float _sun_azimuth = 0.0f;
	float _sun_angle = 0.0f;
	st_mat4f _sun_view;
	st_mat4f _sun_projection;

	// Atmosphere.
	struct st_atmosphere_params* _atmosphere = nullptr;

	// TODO: This will eventually need to turn into a whole management system.
	struct st_sphere_light* _light = nullptr;

	// Somewhat of a hack to make collision stable when stepping with a paused simulation.
	bool _single_step = false;

	// Graphics state.
	uint32_t _frame_index = 0;
};
