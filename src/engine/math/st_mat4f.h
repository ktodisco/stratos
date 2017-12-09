#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_quatf.h"
#include "math/st_vec3f.h"
#include "math/st_vec4f.h"

/*
** Floating point 4x4 matrix.
*/
struct st_mat4f
{
	float data[4][4];

	/*
	** Build an identity matrix.
	*/
	void make_identity();

	/*
	** Build a translation matrix.
	*/
	void make_translation(const st_vec3f& __restrict t);

	/*
	** Build a uniform scaling matrix.
	*/
	void make_scaling(float s);

	/*
	** Build a rotation matrix.
	*/
	void make_rotation(const st_quatf& __restrict q);

	/*
	** Apply translation to the given matrix.
	*/
	void translate(const st_vec3f& __restrict t);

	/*
	** Apply uniform scaling to the given matrix.
	*/
	void scale(float s);

	/*
	** Apply rotation to the given matrix.
	*/
	void rotate(const st_quatf& __restrict q);

	/*
	** Multiply two matrices and store the result in a third.
	*/
	st_mat4f operator*(const st_mat4f& __restrict b) const;

	/*
	** Multiply a matrix by another, storing the result in the first.
	*/
	st_mat4f& operator*=(const st_mat4f& __restrict m);

	/*
	** Transform a vector by a matrix.
	*/
	st_vec4f transform(const st_vec4f& __restrict in) const;

	/*
	** Transforms a vector by a matrix.
	** 
	** This method is similar to transform_point but it ignores 
	** translation as translating a vector is nonsensical.
	*/
	st_vec3f transform_vector(const st_vec3f& __restrict in) const;

	/*
	** Transforms a point by a matrix.
	*/
	st_vec3f transform_point(const st_vec3f& __restrict in) const;

	/*
	** Transpose a matrix.
	*/
	void transpose();

	/*
	** Invert the given matrix.
	*/
	void invert();

	/*
	** Return the inverse of this matrix.
	*/
	st_mat4f inverse() const;

	/*
	** Build a orthographic projection matrix.
	*/
	void make_orthographic(float left, float right, float bottom, float top, float z_near, float z_far);

	/*
	** Build a right-handed perspective projection matrix.
	*/
	void make_perspective_rh(float angle, float aspect, float z_near, float z_far);

	/*
	** Build a right-handed model-view matrix.
	*/
	void make_lookat_rh(const st_vec3f& __restrict eye, const st_vec3f& __restrict at, const st_vec3f& __restrict up);

	/*
	** Determine if two matrices are largely equivalent.
	*/
	bool equal(const st_mat4f& __restrict b);

	/*
	** Get the translation portion of the matrix.
	**
	** The fourth column of the matrix.
	*/
	st_vec3f get_translation() const;

	/*
	** Set the translation portion of the matrix.
	**
	** The fourth column of the matrix.
	*/
	void set_translation(const st_vec3f& translation);

	/*
	** Get the forward vector from the matrix.
	**
	** The first column of the matrix.
	*/
	st_vec3f get_forward() const;

	/*
	** Get the up vector from the matrix.
	**
	** The second column of the matrix.
	*/
	st_vec3f get_up() const;

	/*
	** Get the right vector from the matrix.
	**
	** The third column of the matrix.
	*/
	st_vec3f get_right() const;
};
