#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_vec3f.h"
#include "math/st_vec4f.h"
#include "math/st_math.h"

/*
** Floating point quaternion object.
*/
struct st_quatf
{
	union
	{
		struct { float x, y, z, w; };
		struct { st_vec3f v3; float s; };
		st_vec4f v4;
		float axes[4];
	};

	/*
	** Build a quaternion for an axis and angle.
	** @param axis Normalized axis.
	** @param angle Angle about axis in radians.
	*/
	inline void make_axis_angle(const st_vec3f& __restrict axis, float angle)
	{
		s = st_cosf(angle * 0.5f);
		v3 = axis.scale_result(st_sinf(angle * 0.5f));
	}

	/*
	** Multiply this quaternion by another.
	** @param b The second quaternion.
	** @returns The quaternion representing this * b.
	*/
	inline st_quatf operator*(const st_quatf& __restrict b) const
	{
		st_quatf result;
		result.v3 = st_vec3f_cross(v3, b.v3);

		st_vec3f tmp1;
		tmp1 = b.v3.scale_result(s);
		result.v3 += tmp1;
		st_vec3f tmp2;
		tmp2 = v3.scale_result(b.s);
		result.v3 += tmp2;

		result.s = (s * b.s) - v3.dot(b.v3);

		return result;
	}

	/*
	** Add another quaternion to this one.
	** @param b The second quaternion.
	** @returns The quaternion representing this + b.
	*/
	inline st_quatf operator+(const st_quatf& __restrict b) const
	{
		st_quatf result;
		result.v4 = v4 + b.v4;
		return result;
	}

	/*
	** In-place addition of another quaternion to this.
	** @param b The second quaternion.
	** @returns A reference to this quaternion, as the result of this + b.
	*/
	inline st_quatf& operator+=(const st_quatf& __restrict b)
	{
		(*this) = (*this) + b;
		return (*this);
	}

	/*
	** Return a new quaternion which is this quaternion scaled by s.
	** @param s The float value to scale by.
	** @returns The quaternion equivalent to this scaled by s.
	*/
	inline st_quatf scale_result(const float s) const
	{
		st_quatf result;
		result.v4 = v4.scale_result(s);
		return result;
	}

	/*
	** Conjugate a quaternion in place.
	** @param q The quaternion.
	*/
	inline void conjugate()
	{
		v3.negate();
	}

	/*
	** Invert a quaternion in place.
	** @param q The quaternion.
	*/
	inline void inverse()
	{
		conjugate();
		v4.scale(1.0f / v4.mag2());
	}

	/*
	** Normalize the quaternion.
	*/
	inline void normalize()
	{
		v4.normalize();
	}
};
