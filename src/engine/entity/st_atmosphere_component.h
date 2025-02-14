#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_component.h>

#include <math/st_vec2f.h>
#include <math/st_vec3f.h>
#include <math/st_vec4f.h>

struct st_atmosphere_params
{
	float _planet_radius_km = 0.0f;
	float _atmosphere_radius_km = 0.0f;
	st_vec3f _rayleigh_coefficients = st_vec3f::zero_vector();
	float _rayleigh_average = 0.0f;
	st_vec3f _mie_coefficients = st_vec3f::zero_vector();
	float _mie_average = 0.0f;
	st_vec3f _ozone_coefficients = st_vec3f::zero_vector();
	float _ozone_average = 0.0f;
};

class st_atmosphere_component : public st_component
{
public:

	st_atmosphere_component(
		class st_entity* ent,
		st_vec2f radii,
		st_vec4f rayleigh_params,
		st_vec4f mie_params,
		st_vec4f ozone_params);
	~st_atmosphere_component() {}

	void update(struct st_frame_params* params) override;

	void debug() override;

private:
	st_atmosphere_params _params;
};