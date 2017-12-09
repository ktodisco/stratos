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
#include "entity/st_hello_component.h"
#include "entity/st_lua_component.h"
#include "graphics/st_animation.h"
#include "graphics/st_animation_component.h"
#include "graphics/st_cube_component.h"
#include "graphics/st_egg_parser.h"
#include "graphics/st_material.h"
#include "graphics/st_model_component.h"
#include "graphics/st_ply_parser.h"
#include "graphics/st_geometry.h"
#include "graphics/st_program.h"
#include "gui/st_font.h"
#include "physics/st_physics_component.h"
#include "physics/st_playermove_component.h"
#include "physics/st_physics_world.h"
#include "physics/st_rigid_body.h"
#include "physics/st_shape.h"

#include "gui/st_button.h"
#include "gui/st_checkbox.h"
#include "gui/st_label.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#if defined(st_MINGW)
#include <unistd.h>
#endif

st_font* g_font = nullptr;

static void set_root_path(const char* exepath);

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);

	st_job::startup(0xffff, 256, 256);

	// Create objects for three phases of the frame: input, sim and output.
	st_input* input = new st_input();
	st_sim* sim = new st_sim();
	st_physics_world* world = new st_physics_world();
	st_output* output = new st_output(input->get_window());

	// Create camera.
	st_camera* camera = new st_camera({ 0.0f, 7.0f, 20.0f });
	st_quatf rotation;
	rotation.make_axis_angle(st_vec3f::y_vector(), st_degrees_to_radians(180.0f));
	camera->rotate(rotation);
	rotation.make_axis_angle(st_vec3f::x_vector(), st_degrees_to_radians(15.0f));
	camera->rotate(rotation);

	// Create the default font:
	g_font = new st_font("VeraMono.ttf", 16.0f, 512, 512);

	st_model test_model;
	ply_to_model("data/models/bunny_med_res.ply", &test_model);

	st_entity test_entity;
	st_phong_material* test_material = new st_phong_material();
	test_material->set_color({0.5f, 0.5f, 0.5f});
	st_model_component model_component(&test_entity, &test_model, test_material);
	sim->add_entity(&test_entity);

	// The bunny is rather small, so scale.
	test_entity.scale(40.0f);

	// Main loop:
	while (true)
	{
		// We pass frame state through the 3 phases using a params object.
		st_frame_params params;

		// Gather user input and current time.
		if (!input->update(&params))
		{
			break;
		}

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
#if defined(st_MSVC)
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
#elif defined(st_MINGW)
	char* cwd;
	char buf[PATH_MAX + 1];
	cwd = getcwd(buf, PATH_MAX + 1);
	strcpy_s(g_root_path, sizeof(g_root_path), cwd);

	g_root_path[strlen(cwd)] = '/';
	g_root_path[strlen(cwd) + 1] = '\0';
#endif
}
