/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_camera.h>

#include <framework/st_frame_params.h>
#include <framework/st_input.h>

#include <math/st_math.h>

st_camera::st_camera(const st_vec3f& eye, uint32_t width, uint32_t height)
	: _width(width), _height(height)
{
	_transform.make_translation(eye);
}

st_camera::~st_camera()
{
}

void st_camera::update(st_frame_params* params)
{
	const float k_move_speed = 0.1f;
	const float k_rotate_speed = 0.5f;

	// Use WASD to control the position.
	st_vec3f translation = { 0.0f, 0.0f, 0.0f };
	translation += (params->_button_mask & k_button_d) ? -_transform.get_right() : st_vec3f::zero_vector();
	translation += (params->_button_mask & k_button_a) ? _transform.get_right() : st_vec3f::zero_vector();
	translation += (params->_button_mask & k_button_s) ? -_transform.get_forward() : st_vec3f::zero_vector();
	translation += (params->_button_mask & k_button_w) ? _transform.get_forward() : st_vec3f::zero_vector();
	translation += (params->_button_mask & k_button_q) ? -_transform.get_up() : st_vec3f::zero_vector();
	translation += (params->_button_mask & k_button_e) ? _transform.get_up() : st_vec3f::zero_vector();
	translation.scale(k_move_speed);

	// By using the camera's directional vectors, we've defined the translation in world space.
	st_mat4f world_translation;
	world_translation.make_translation(translation);
	_transform = _transform * world_translation;

	_position = _transform.get_translation();

	// Use arrow keys to pitch and rotate.
	float rotation = 0.0f;
	rotation += (params->_button_mask & k_button_left) ? k_rotate_speed : 0.0f;
	rotation += (params->_button_mask & k_button_right) ? -k_rotate_speed : 0.0f;
	float pitch = 0.0f;
	pitch += (params->_button_mask & k_button_up) ? -k_rotate_speed : 0.0f;
	pitch += (params->_button_mask & k_button_down) ? k_rotate_speed : 0.0f;

	_yaw += rotation;
	_pitch += pitch;

	// Recreate the transform.
	_transform.make_identity();

	st_quatf rotation_axis_angle;
	rotation_axis_angle.make_axis_angle(st_vec3f::y_vector(), st_degrees_to_radians(_yaw));
	st_mat4f yaw_matrix;
	yaw_matrix.make_rotation(rotation_axis_angle);

	rotation_axis_angle.make_axis_angle(st_vec3f::x_vector(), st_degrees_to_radians(_pitch));
	st_mat4f pitch_matrix;
	pitch_matrix.make_rotation(rotation_axis_angle);
	_transform = pitch_matrix * (yaw_matrix * _transform);

	_transform.translate(_position);

	st_vec3f eye = _transform.get_translation();
	st_vec3f at = eye + _transform.get_forward();
	st_vec3f up = st_vec3f::y_vector();

	st_mat4f view;
	view.make_lookat_rh(eye, at, up);

	st_mat4f projection;
	projection.make_perspective_rh(st_degrees_to_radians(45.0f), (float)_width / (float)_height, 0.1f, 10000.0f);

#if defined(ST_GRAPHICS_API_VULKAN)
	// Vulkan requires a correction to the projection matrix to account for both framebuffer coordinate
	// and depth range changes.
	st_mat4f vk_correction =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};
	projection = vk_correction * projection;
#endif

	params->_width = _width;
	params->_height = _height;
	params->_view = view;
	params->_projection = projection;
	params->_eye = eye;
}
