/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/animation/st_animation_component.h>

#include <entity/st_entity.h>

#include <graphics/animation/st_animation.h>
#include <graphics/geometry/st_debug_geometry.h>
#include <graphics/geometry/st_model_data.h>

#include <cassert>

st_animation_component::st_animation_component(st_entity* ent, st_model_data* model) : st_component(ent)
{
	_skeleton = model->_skeleton;
	assert(_skeleton != 0);
}

st_animation_component::~st_animation_component()
{
	_skeleton = 0;

	if (_playing)
	{
		delete _playing;
	}
}

void st_animation_component::update(st_frame_params* params)
{
	if (_playing)
	{
		_playing->_time += std::chrono::duration_cast<std::chrono::milliseconds>(params->_delta_time);

		float local_time = (_playing->_time.count() % 1000) / 1000.0f;
		uint32_t frame = (uint32_t)(local_time * _playing->_animation->_rate);
		// Safety.
		frame = frame % _playing->_animation->_rate;

		// For now, no interpolation. Select the closest frame.
		for (uint32_t joint_index = 0; joint_index < _skeleton->_joints.size(); ++joint_index)
		{
			st_joint* j = _skeleton->_joints[joint_index];

			st_mat4f parent_matrix;
			parent_matrix.make_identity();
			if (j->_parent < INT_MAX)
			{
				parent_matrix = _skeleton->_joints[j->_parent]->_world;
			}
			j->_world = _playing->_animation->_poses[frame]._transforms[joint_index] * parent_matrix;
			j->_skin = j->_inv_bind * j->_world;
		}
	}
	
#if DEBUG_DRAW_SKELETON
	for (uint32_t joint_index = 0; joint_index < _skeleton->_joints.size(); ++joint_index)
	{
		st_joint* j = _skeleton->_joints[joint_index];

		st_dynamic_drawcall drawcall;
		draw_debug_sphere(0.4f, j->_world * get_entity()->get_transform(), &drawcall);

		while (params->_dynamic_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
		params->_dynamic_drawcalls.push_back(drawcall);
		params->_dynamic_drawcall_lock.clear(std::memory_order_release);
	}
#endif
}

void st_animation_component::play(st_animation* animation)
{
	_playing = new st_animation_playback();
	_playing->_animation = animation;
	_playing->_time == std::chrono::milliseconds::zero();
}
