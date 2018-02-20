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

st_font* g_font = nullptr;

static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	st_job::startup(0xffff, 256, 256);

	st_input* input = new st_input();

	// Create a window.
	st_window* window = new st_window("Stratos", 1280, 720, input);

	// Create the rendering context for the window.
	st_render_context* render = new st_render_context(window);

	// Create the shader manager, loading all the shaders.
	std::unique_ptr<st_shader_manager> shader_manager =
		std::make_unique<st_shader_manager>();

	// Create objects for phases of the frame: sim, physics, and output.
	st_sim* sim = new st_sim();
	st_physics_world* world = new st_physics_world();
	st_output* output = new st_output(window, render);

	// Create camera.
	st_camera* camera = new st_camera({ 0.0f, 7.0f, 20.0f });
	st_quatf rotation;
	rotation.make_axis_angle(st_vec3f::y_vector(), st_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(st_vec3f::x_vector(), st_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create the default font:
	g_font = new st_font("VeraMono.ttf", 16.0f, 512, 512);

	// Set up a test for the phong material.
	st_model_data test_model;
	ply_to_model("data/models/bunny_med_res.ply", &test_model);

	st_entity test_entity;
	st_phong_material* test_material = new st_phong_material();
	//test_material->set_color({0.5f, 0.5f, 0.5f});
	st_model_component model_component(&test_entity, &test_model, test_material);
	sim->add_entity(&test_entity);

	// The bunny is rather small, so scale.
	test_entity.scale(40.0f);
	test_entity.translate({ -2.0f, 0.0f, 0.0f });

	// Set up a test for the unlit texture material.
	st_model_data unlit_model;
	egg_to_model("data/models/cube.egg", &unlit_model);

	st_entity unlit_entity;
	st_unlit_texture_material* unlit_material = new st_unlit_texture_material("data/textures/test.png");
	st_model_component unlit_model_component(&unlit_entity, &unlit_model, unlit_material);
	sim->add_entity(&unlit_entity);

	unlit_entity.translate({ 2.0f, 0.0f, 0.0f });

	window->show();

	st_render_context::get()->end_loading();

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

	delete output;
	delete world;
	delete sim;
	delete input;
	delete camera;

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
