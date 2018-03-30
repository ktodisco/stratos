/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "framework/st_camera.h"
#include "framework/st_compiler_defines.h"
#include "framework/st_input.h"
#include "framework/st_sim.h"
#include "framework/st_output.h"
#include "jobs/st_job.h"

#include "entity/st_entity.h"
#include "entity/st_lua_component.h"
#include "graphics/st_animation.h"
#include "graphics/st_animation_component.h"
#include "graphics/st_egg_parser.h"
#include "graphics/st_gbuffer_material.h"
#include "graphics/st_material.h"
#include "graphics/st_model_component.h"
#include "graphics/st_ply_parser.h"
#include "graphics/st_model_data.h"
#include "graphics/st_render_context.h"
#include "graphics/st_shader_manager.h"
#include "gui/st_font.h"
#include "physics/st_physics_component.h"
#include "physics/st_playermove_component.h"
#include "physics/st_physics_world.h"
#include "physics/st_rigid_body.h"
#include "physics/st_shape.h"
#include "system/st_window.h"

#include "gui/st_button.h"
#include "gui/st_checkbox.h"
#include "gui/st_label.h"

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
	std::unique_ptr<st_camera> camera = std::make_unique<st_camera>(st_vec3f({ 0.0f, 7.0f, 20.0f }));
	st_quatf rotation;
	rotation.make_axis_angle(st_vec3f::y_vector(), st_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(st_vec3f::x_vector(), st_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create the default font:
	g_font = std::make_unique<st_font>("VeraMono.ttf", 16.0f, 512, 512);

	// Set up a test for the unlit texture material.
	st_model_data pbr_model;
	ply_to_model("data/models/sphere.ply", &pbr_model);

	st_entity pbr_entity;
	st_gbuffer_material pbr_material("");
	st_model_component unlit_model_component(&pbr_entity, &pbr_model, &pbr_material);
	sim->add_entity(&pbr_entity);

	pbr_entity.scale(4.0f);

	st_model_data floor_model;
	ply_to_model("data/models/plane.ply", &floor_model);

	st_entity floor_entity;
	st_gbuffer_material floor_material("data/textures/floor.png");
	st_model_component floor_model_component(&floor_entity, &floor_model, &floor_material);
	sim->add_entity(&floor_entity);

	floor_entity.scale(5.0f);

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
