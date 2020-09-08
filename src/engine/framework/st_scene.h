#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <math/st_vec3f.h>

#include <memory>
#include <vector>

class st_scene
{
public:
	st_scene();
	~st_scene();

	class st_entity* add_entity(
		class st_sim* sim,
		st_vec3f position,
		struct st_model_data* model,
		const char* albedo,
		const char* mre,
		float emissive);
	class st_entity* add_light(
		class st_sim* sim,
		st_vec3f position,
		st_vec3f color,
		float power,
		struct st_model_data* model,
		const char* albedo,
		const char* mre);

	void setup_lighting_test(class st_sim* sim);

private:
	std::vector<std::unique_ptr<class st_entity>> _entities;
};
