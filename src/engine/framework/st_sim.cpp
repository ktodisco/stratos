/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_sim.h>

#include <entity/st_entity.h>

#include <framework/st_compiler_defines.h>

#include <jobs/st_job.h>

#if defined(ST_MINGW)
#include <malloc.h>
#endif

st_sim::st_sim()
{
}

st_sim::~st_sim()
{
}

void st_sim::add_entity(st_entity* ent)
{
	_entities.push_back(ent);
}

void st_sim::update(st_frame_params* params)
{
	// Create jobs that update all entities in parallel (one job per entity).
	// There are 2 components:
	// 1. The job declarations; a function and a pointer to data for that function.
	// 2. The data for each job; an entity and the frame_params.

	auto decls = static_cast<st_job_decl_t*>(alloca(sizeof(st_job_decl_t) * _entities.size()));

	struct update_data_t
	{
		st_entity* _entity;
		st_frame_params* _params;
	};
	auto update_data = static_cast<update_data_t*>(alloca(sizeof(update_data_t) * _entities.size()));

	for (int i = 0; i < _entities.size(); ++i)
	{
		update_data[i]._entity = _entities[i];
		update_data[i]._params = params;

		decls[i]._data = update_data + i;
		decls[i]._entry = [](void* data)
		{
			auto update_data = static_cast<update_data_t*>(data);
			update_data->_entity->update(update_data->_params);
		};
	}

	// Dispatch the jobs:
	int32_t update_counter;
	st_job::run(decls, int(_entities.size()), &update_counter);
	st_job::wait(&update_counter);
}

void st_sim::late_update(st_frame_params* params)
{
	auto decls = static_cast<st_job_decl_t*>(alloca(sizeof(st_job_decl_t) * _entities.size()));

	struct update_data_t
	{
		st_entity* _entity;
		st_frame_params* _params;
	};
	auto update_data = static_cast<update_data_t*>(alloca(sizeof(update_data_t) * _entities.size()));

	for (int i = 0; i < _entities.size(); ++i)
	{
		update_data[i]._entity = _entities[i];
		update_data[i]._params = params;

		decls[i]._data = update_data + i;
		decls[i]._entry = [](void* data)
		{
			auto update_data = static_cast<update_data_t*>(data);
			update_data->_entity->late_update(update_data->_params);
		};
	}

	int32_t update_counter;
	st_job::run(decls, int(_entities.size()), &update_counter);
	st_job::wait(&update_counter);
}
