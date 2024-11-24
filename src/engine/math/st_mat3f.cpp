/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "math/st_mat3f.h"

#include "math/st_math.h"

void st_mat3f::make_identity()
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			data[i][j] = i == j ? 1.0f : 0.0f;
		}
	}
}

void st_mat3f::make_translation(const st_vec2f& __restrict t)
{
	make_identity();
	data[0][2] = t.x;
	data[1][2] = t.y;
	data[2][2] = 1.0f;
}

void st_mat3f::make_scaling(float s)
{
	make_identity();
	for (int i = 0; i < 2; ++i)
	{
		data[i][i] = s;
	}
	data[2][2] = 1.0f;
}

void st_mat3f::make_rotation_z(float angle)
{
	make_identity();

	float c = st_cosf(angle);
	float s = st_sinf(angle);

	data[0][0] = c;
	data[1][0] = s;
	data[0][1] = -s;
	data[2][1] = c;
	data[2][2] = 1.0f;
}

void st_mat3f::translate(const st_vec2f& __restrict t)
{
	st_mat3f tmp;
	tmp.make_translation(t);
	(*this) *= tmp;
}

void st_mat3f::scale(float s)
{
	st_mat3f tmp;
	tmp.make_scaling(s);
	(*this) *= tmp;
}

void st_mat3f::rotate_z(float angle)
{
	st_mat3f tmp;
	tmp.make_rotation_z(angle);
	(*this) *= tmp;
}

st_mat3f st_mat3f::operator*(const st_mat3f& __restrict b) const
{
	st_mat3f result;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			float tmp = 0.0f;
			for (int k = 0; k < 3; ++k)
			{
				tmp += data[k][i] * b.data[j][k];
			}
			result.data[j][i] = tmp;
		}
	}
	return result;
}

st_mat3f& st_mat3f::operator*=(const st_mat3f& __restrict m)
{
	(*this) = (*this) * m;
	return (*this);
}

st_vec3f st_mat3f::transform(const st_vec3f& __restrict in) const
{
	st_vec3f result;
	result.x = in.x * data[0][0] + in.y * data[0][1] + in.z * data[0][2];
	result.y = in.x * data[1][0] + in.y * data[1][1] + in.z * data[1][2];
	result.z = in.x * data[2][0] + in.y * data[2][1] + in.z * data[2][2];
	return result;
}

void st_mat3f::transpose()
{
	st_mat3f tmp;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			tmp.data[i][j] = data[j][i];
		}
	}
	(*this) = tmp;
}

void st_mat3f::invert()
{
	st_mat3f tmp;
	tmp.data[0][0] =  data[1][1] * data[2][2] - data[1][2] * data[2][1];
	tmp.data[0][1] = -data[1][0] * data[2][2] + data[1][2] * data[2][0];
	tmp.data[0][2] =  data[1][0] * data[2][1] - data[1][1] * data[2][0];
	tmp.data[1][0] = -data[0][1] * data[2][2] + data[2][1] * data[0][2];
	tmp.data[1][1] =  data[0][0] * data[2][2] - data[2][0] * data[0][2];
	tmp.data[1][2] = -data[0][0] * data[2][1] + data[2][0] * data[0][1];
	tmp.data[2][0] =  data[0][1] * data[1][2] - data[1][1] * data[0][2];
	tmp.data[2][1] = -data[0][0] * data[1][2] + data[1][0] * data[0][2];
	tmp.data[2][2] =  data[0][0] * data[1][1] - data[1][0] * data[0][1];

	float inv_det = data[0][0] * tmp.data[0][0] + data[0][1] * tmp.data[0][1] + data[0][2] * tmp.data[0][2];
	// TODO: Assert inv_det is not 0.
	inv_det = 1.0f / inv_det;

	data[0][0] = tmp.data[0][0] * inv_det;
	data[0][1] = tmp.data[1][0] * inv_det;
	data[0][2] = tmp.data[2][0] * inv_det;
	data[1][0] = tmp.data[0][1] * inv_det;
	data[1][1] = tmp.data[1][1] * inv_det;
	data[1][2] = tmp.data[2][1] * inv_det;
	data[2][0] = tmp.data[0][2] * inv_det;
	data[2][1] = tmp.data[1][2] * inv_det;
	data[2][2] = tmp.data[2][2] * inv_det;
}

bool st_mat3f::equal(const st_mat3f& __restrict b)
{
	bool is_not_equal = false;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			is_not_equal = is_not_equal || !st_equalf(data[i][j], b.data[i][j]);
		}
	}
	return !is_not_equal;
}
