/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_entity.h>
#include <entity/st_lua_component.h>

#include <framework/st_camera.h>
#include <framework/st_compiler_defines.h>
#include <framework/st_input.h>
#include <framework/st_sim.h>
#include <framework/st_output.h>

#include <graphics/animation/st_animation.h>
#include <graphics/animation/st_animation_component.h>
#include <graphics/parse/st_egg_parser.h>
#include <graphics/material/st_gbuffer_material.h>
#include <graphics/material/st_material.h>
#include <graphics/material/st_parallax_occlusion_material.h>
#include <graphics/geometry/st_model_component.h>
#include <graphics/parse/st_ply_parser.h>
#include <graphics/geometry/st_model_data.h>
#include <graphics/st_render_context.h>
#include <graphics/st_shader_manager.h>

#include <gui/st_button.h>
#include <gui/st_checkbox.h>
#include <gui/st_font.h>
#include <gui/st_label.h>

#include <jobs/st_job.h>

#include <physics/st_physics_component.h>
#include <physics/st_playermove_component.h>
#include <physics/st_physics_world.h>
#include <physics/st_rigid_body.h>
#include <physics/st_shape.h>

#include <system/st_window.h>

#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(ST_MINGW)
#include <unistd.h>
#endif

std::unique_ptr<st_font> g_font = nullptr;

static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	st_job::startup(0xffff, 256, 256);

	std::unique_ptr<st_input> input = std::make_unique<st_input>();

	// Create a window.
	std::unique_ptr<st_window> window = std::make_unique<st_window>("Stratos Renderer", 1280, 720, input.get());

	// Create the rendering context for the window.
	std::unique_ptr<st_render_context> render = std::make_unique<st_render_context>(window.get());

	// Create the shader manager, loading all the shaders.
	std::unique_ptr<st_shader_manager> shader_manager =
		std::make_unique<st_shader_manager>();

	// Create objects for phases of the frame: sim, physics, and output.
	std::unique_ptr<st_sim> sim = std::make_unique<st_sim>();
	std::unique_ptr<st_physics_world> world = std::make_unique<st_physics_world>();
	std::unique_ptr<st_output> output = std::make_unique<st_output>(window.get(), render.get());

	// Create camera.
	std::unique_ptr<st_camera> camera = std::make_unique<st_camera>(st_vec3f({ 0.0f, 1.8f, 3.5f }));
	camera->set_yaw(180.0f);
	camera->set_pitch(20.0f);

	// Create the default font:
	g_font = std::make_unique<st_font>("VeraMono.ttf", 16.0f, 512, 512);

	// Set up a test for the unlit texture material.
	st_model_data sphere_model;
	ply_to_model("data/models/sphere.ply", &sphere_model);

	st_model_data plane_model;
	ply_to_model("data/models/plane.ply", &plane_model);

	st_entity floor_entity;
	st_gbuffer_material floor_material(
		"data/textures/floor.png",
		"data/textures/dielectric_25_roughness.png");
	st_model_component floor_model_component(&floor_entity, &plane_model, &floor_material);
	sim->add_entity(&floor_entity);

	floor_entity.scale(0.5f);

	st_entity dr0_entity;
	st_gbuffer_material dr0_material(
		"data/textures/white_albedo.png",
		"data/textures/dielectric_0_roughness.png");
	st_model_component dr0_model(&dr0_entity, &sphere_model, &dr0_material);
	sim->add_entity(&dr0_entity);
	dr0_entity.translate({ -3.0f, 1.0f, 0.0f });

	st_entity dr25_entity;
	st_gbuffer_material dr25_material(
		"data/textures/white_albedo.png",
		"data/textures/dielectric_25_roughness.png");
	st_model_component dr25_model(&dr25_entity, &sphere_model, &dr25_material);
	sim->add_entity(&dr25_entity);
	dr25_entity.translate({ -1.5f, 1.0f, 0.0f });

	st_entity dr50_entity;
	st_gbuffer_material dr50_material(
		"data/textures/white_albedo.png",
		"data/textures/dielectric_50_roughness.png");
	st_model_component dr50_model(&dr50_entity, &sphere_model, &dr50_material);
	sim->add_entity(&dr50_entity);
	dr50_entity.translate({ 0.0f, 1.0f, 0.0f });

	st_entity dr75_entity;
	st_gbuffer_material dr75_material(
		"data/textures/white_albedo.png",
		"data/textures/dielectric_75_roughness.png");
	st_model_component dr75_model(&dr75_entity, &sphere_model, &dr75_material);
	sim->add_entity(&dr75_entity);
	dr75_entity.translate({ 1.5f, 1.0f, 0.0f });

	st_entity dr100_entity;
	st_gbuffer_material dr100_material(
		"data/textures/white_albedo.png",
		"data/textures/dielectric_100_roughness.png");
	st_model_component dr100_model(&dr100_entity, &sphere_model, &dr100_material);
	sim->add_entity(&dr100_entity);
	dr100_entity.translate({ 3.0f, 1.0f, 0.0f });

	st_entity light_entity;
	st_gbuffer_material light_material(
		"data/textures/white_albedo.png",
		"data/textures/default_emissive.png");
	light_material.set_emissive(200.0f);
	st_model_component light_model_component(&light_entity, &sphere_model, &light_material);
	sim->add_entity(&light_entity);

	light_entity.translate({ 0.0f, 1.0f, 3.0f });
	light_entity.scale(0.05f);

	window->show();

	// Main loop:
	while (true)
	{
		// Pump messages.
		if (!window->update())
		{
			break;
		}

		// We pass frame state through the 3 phases using a params object.
		st_frame_params params;

		// Gather user input and current time.
		input->update(&params);

		// Update the camera.
		camera->update(&params);

		// Run gameplay.
		sim->update(&params);

		// Step the physics world.
		world->step(&params);

		// Perform the late update.
		sim->late_update(&params);

		// Draw to screen.
		output->update(&params);
	}

	st_job::shutdown();

	return 0;
}

char g_root_path[256];
static void set_root_path(const char* exepath)
{
#if defined(ST_MSVC)
	strcpy_s(g_root_path, sizeof(g_root_path), exepath);

	// Strip the executable file name off the end of the path:
	char* slash = strrchr(g_root_path, '\\');
	if (!slash)
	{
		slash = strrchr(g_root_path, '/');
	}
	if (slash)
	{
		slash[1] = '\0';
	}
#elif defined(ST_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
