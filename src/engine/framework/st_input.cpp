/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "st_input.h"
#include "st_compiler_defines.h"
#include "st_frame_params.h"

#include <cassert>
#include <cstdio>
#if defined(st_MINGW)
#include <unistd.h>
#else
#include <thread>
#endif

#define SDL_MAIN_HANDLED
#include <SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>

st_input::st_input() : _paused(false)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER);

	_window = SDL_CreateWindow(
		"Game Architecture Engine",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		1280,
		720,
		SDL_WINDOW_OPENGL);
	assert(_window);

	// Request OpenGL 4.4 Context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GLContext context = SDL_GL_CreateContext(static_cast<SDL_Window* >(_window));
	SDL_GL_SetSwapInterval(0);

	SDL_GL_MakeCurrent((SDL_Window*)_window, context);

	int major_version;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major_version);
	int minor_version;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor_version);

	printf("Using OpenGL %d.%d.\n", major_version, minor_version);
	
	// Init GLEW after creating the OpenGL context so it can locate the functions.
	GLint glew_init_result = glewInit();
	if (glew_init_result != GLEW_OK)
	{
		printf("Failed to initialize GLEW. %s. Exiting.\n", glewGetErrorString(glew_init_result));
		exit(1);
	}

	_button_mask = 0;
	_mouse_button_mask = 0;
	_mouse_x = 0.0f;
	_mouse_y = 0.0f;
	_last_time = std::chrono::high_resolution_clock::now();
}

st_input::~st_input()
{
	SDL_DestroyWindow(static_cast<SDL_Window* >(_window));
	SDL_Quit();
}

bool st_input::update(st_frame_params* params)
{
	bool result = true;

	// Save the previous frame's button mask to computed released keys.
	uint64_t previous_button_mask = _button_mask;
	params->_mouse_click_mask = 0;

	// Process events. Gather button status.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			_mouse_x = (float)event.motion.x;
			_mouse_y = (float)event.motion.y;
			break;
		case SDL_MOUSEBUTTONDOWN:
			_mouse_button_mask |= uint64_t(1) << event.button.button;
			break;
		case SDL_MOUSEBUTTONUP:
			_mouse_button_mask &= ~(uint64_t(1) << event.button.button);
			params->_mouse_click_mask |= uint64_t(1) << event.button.button;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT: _button_mask |= k_button_left; break;
			case SDLK_RIGHT: _button_mask |= k_button_right; break;
			case SDLK_UP: _button_mask |= k_button_up; break;
			case SDLK_DOWN: _button_mask |= k_button_down; break;
			case SDLK_SPACE: _button_mask |= k_button_space; break;
			case SDLK_a: _button_mask |= k_button_a; break;
			case SDLK_b: _button_mask |= k_button_b; break;
			case SDLK_c: _button_mask |= k_button_c; break;
			case SDLK_d: _button_mask |= k_button_d; break;
			case SDLK_e: _button_mask |= k_button_e; break;
			case SDLK_f: _button_mask |= k_button_f; break;
			case SDLK_g: _button_mask |= k_button_g; break;
			case SDLK_h: _button_mask |= k_button_h; break;
			case SDLK_i: _button_mask |= k_button_i; break;
			case SDLK_j: _button_mask |= k_button_j; break;
			case SDLK_k: _button_mask |= k_button_k; break;
			case SDLK_l: _button_mask |= k_button_l; break;
			case SDLK_m: _button_mask |= k_button_m; break;
			case SDLK_n: _button_mask |= k_button_n; break;
			case SDLK_o: _button_mask |= k_button_o; break;
			case SDLK_p: _button_mask |= k_button_p; break;
			case SDLK_q: _button_mask |= k_button_q; break;
			case SDLK_r: _button_mask |= k_button_r; break;
			case SDLK_s: _button_mask |= k_button_s; break;
			case SDLK_t: _button_mask |= k_button_t; break;
			case SDLK_u: _button_mask |= k_button_u; break;
			case SDLK_v: _button_mask |= k_button_v; break;
			case SDLK_w: _button_mask |= k_button_w; break;
			case SDLK_x: _button_mask |= k_button_x; break;
			case SDLK_y: _button_mask |= k_button_y; break;
			case SDLK_z: _button_mask |= k_button_z; break;
			default: break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT: _button_mask &= ~k_button_left; break;
			case SDLK_RIGHT: _button_mask &= ~k_button_right; break;
			case SDLK_UP: _button_mask &= ~k_button_up; break;
			case SDLK_DOWN: _button_mask &= ~k_button_down; break;
			case SDLK_SPACE: _button_mask &= ~k_button_space; break;
			case SDLK_a: _button_mask &= ~k_button_a; break;
			case SDLK_b: _button_mask &= ~k_button_b; break;
			case SDLK_c: _button_mask &= ~k_button_c; break;
			case SDLK_d: _button_mask &= ~k_button_d; break;
			case SDLK_e: _button_mask &= ~k_button_e; break;
			case SDLK_f: _button_mask &= ~k_button_f; break;
			case SDLK_g: _button_mask &= ~k_button_g; break;
			case SDLK_h: _button_mask &= ~k_button_h; break;
			case SDLK_i: _button_mask &= ~k_button_i; break;
			case SDLK_j: _button_mask &= ~k_button_j; break;
			case SDLK_k: _button_mask &= ~k_button_k; break;
			case SDLK_l: _button_mask &= ~k_button_l; break;
			case SDLK_m: _button_mask &= ~k_button_m; break;
			case SDLK_n: _button_mask &= ~k_button_n; break;
			case SDLK_o: _button_mask &= ~k_button_o; break;
			case SDLK_p: _button_mask &= ~k_button_p; break;
			case SDLK_q: _button_mask &= ~k_button_q; break;
			case SDLK_r: _button_mask &= ~k_button_r; break;
			case SDLK_s: _button_mask &= ~k_button_s; break;
			case SDLK_t: _button_mask &= ~k_button_t; break;
			case SDLK_u: _button_mask &= ~k_button_u; break;
			case SDLK_v: _button_mask &= ~k_button_v; break;
			case SDLK_w: _button_mask &= ~k_button_w; break;
			case SDLK_x: _button_mask &= ~k_button_x; break;
			case SDLK_y: _button_mask &= ~k_button_y; break;
			case SDLK_z: _button_mask &= ~k_button_z; break;
			default: break;
			}
			break;
		case SDL_QUIT:
			result = false;
			break;
		default:
			break;
		}
	}

	_pressed_mask = _button_mask & ~previous_button_mask;
	_released_mask = previous_button_mask & ~_button_mask;

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
#if defined(st_MINGW) && defined(st_32_BIT)
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

	return result;
}
