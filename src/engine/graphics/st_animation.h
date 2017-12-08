#pragma once

/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_mat4f.h"

#include <chrono>
#include <climits>
#include <vector>

struct st_joint
{
	char _name[32];

	st_mat4f _world;
	st_mat4f _inv_bind;
	st_mat4f _skin;

	uint32_t _parent = INT_MAX;
};

struct st_skeleton
{
	static const uint32_t k_max_skeleton_joints = 75;

	std::vector<st_joint*> _joints;
};

struct st_skeleton_pose
{
	std::vector<st_mat4f> _transforms;
};

struct st_animation
{
	float _length;
	uint32_t _rate;

	std::vector<st_skeleton_pose> _poses;
};

struct st_animation_playback
{
	st_animation* _animation;
	std::chrono::milliseconds _time;
};
