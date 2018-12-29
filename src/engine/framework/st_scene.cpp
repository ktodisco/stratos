/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <framework/st_scene.h>

#include <entity/st_entity.h>

#include <framework/st_sim.h>

#include <graphics/material/st_gbuffer_material.h>
#include <graphics/material/st_material.h>
#include <graphics/geometry/st_model_component.h>
#include <graphics/geometry/st_model_data.h>
#include <graphics/parse/st_ply_parser.h>
#include <graphics/st_light_component.h>

#include <import/st_assimp.h>

st_scene::st_scene()
{

}

st_scene::~st_scene()
{
	_entities.clear();
}

st_entity* st_scene::add_entity(
	st_sim* sim,
	st_vec3f position,
	st_model_data* model,
	const char* albedo,
	const char* mre)
{
	std::unique_ptr<st_entity> entity = std::make_unique<st_entity>();
	std::unique_ptr<st_material> material = std::make_unique<st_gbuffer_material>(albedo, mre);
	std::unique_ptr<st_model_component> model_component = std::make_unique<st_model_component>(entity.get(), model, std::move(material));
	entity->add_component(std::move(model_component));
	sim->add_entity(entity.get());

	entity->translate(position);

	_entities.push_back(std::move(entity));

	return _entities[_entities.size() - 1].get();
}

st_entity* st_scene::add_light(
	class st_sim* sim,
	st_vec3f position,
	st_vec3f color,
	float power,
	struct st_model_data* model,
	const char* albedo,
	const char* mre)
{
	st_entity* entity = add_entity(sim, position, model, albedo, mre);

	std::unique_ptr<st_light_component> light_component = std::make_unique<st_light_component>(entity, color , power);
	entity->add_component(std::move(light_component));

	return entity;
}

void st_scene::setup_lighting_test(class st_sim* sim)
{
	st_model_data sphere_model;
	assimp_import_model("data/models/sphere.ply", &sphere_model);
	//ply_to_model("data/models/sphere.ply", &sphere_model);

	st_model_data plane_model;
	ply_to_model("data/models/plane.ply", &plane_model);

	st_entity* floor_entity = add_entity(
		sim,
		{ 0.0f, 0.0f, 0.0f },
		&plane_model,
		"data/textures/floor.png",
		"data/textures/dielectric_25_roughness.png");
	floor_entity->scale(0.5f);

	st_entity* dr0_entity = add_entity(
		sim,
		{ -3.0f, 1.0f, 0.0f },
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/dielectric_0_roughness.png");

	st_entity* dr25_entity = add_entity(
		sim,
		{ -1.5f, 1.0f, 0.0f },
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/dielectric_25_roughness.png");

	st_entity* dr50_entity = add_entity(
		sim,
		{ 0.0f, 1.0f, 0.0f },
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/dielectric_50_roughness.png");

	st_entity* dr75_entity = add_entity(
		sim,
		{ 1.5f, 1.0f, 0.0f },
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/dielectric_75_roughness.png");

	st_entity* dr100_entity = add_entity(
		sim,
		{ 3.0f, 1.0f, 0.0f },
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/dielectric_100_roughness.png");

	st_entity* light_entity = add_light(
		sim,
		{ 0.0f, 1.0f, 3.0f },
		{ 1.0f, 1.0f, 0.9f },
		2400.0f,
		&sphere_model,
		"data/textures/white_albedo.png",
		"data/textures/default_emissive.png");
	light_entity->scale(0.1f);
}
