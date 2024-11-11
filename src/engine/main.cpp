/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <entity/st_entity.h>
#include <entity/st_lua_component.h>

#include <framework/st_camera.h>
#include <framework/st_compiler_defines.h>
#include <framework/st_global_resources.h>
#include <framework/st_input.h>
#include <framework/st_scene.h>
#include <framework/st_sim.h>
#include <framework/st_output.h>

#include <graphics/animation/st_animation.h>
#include <graphics/animation/st_animation_component.h>
#include <graphics/parse/st_egg_parser.h>
#include <graphics/material/st_gbuffer_material.h>
#include <graphics/material/st_material.h>
#include <graphics/material/st_parallax_occlusion_material.h>
#include <graphics/geometry/st_model_component.h>
#include <graphics/geometry/st_model_data.h>
#include <graphics/st_light_component.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_shader_manager.h>

#include <gui/st_button.h>
#include <gui/st_checkbox.h>
#include <gui/st_font.h>
#include <gui/st_imgui.h>
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

st_font* g_font = nullptr;

static void set_root_path(const char* exepath);

e_st_graphics_api get_api(int argc, const char** argv)
{
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-api") == 0 && i < (argc + 1))
		{
			i++;
			if (strcmp(argv[i], "dx12") == 0)
				return e_st_graphics_api::dx12;
			else if (strcmp(argv[i], "opengl") == 0)
				return e_st_graphics_api::opengl;
			else if (strcmp(argv[i], "vulkan") == 0)
				return e_st_graphics_api::vulkan;
		}
	}

	return e_st_graphics_api::dx12;
}

int main(int argc, const char** argv)
{
	set_root_path(argv[0]);
	e_st_graphics_api api = get_api(argc, argv);

	st_job::startup(0xffff, 256, 256);

	std::unique_ptr<st_input> input = std::make_unique<st_input>();

	// Create a window.
	std::unique_ptr<st_window> window = std::make_unique<st_window>("Stratos Renderer", 1280, 720, input.get());

	// Create the graphics context for the window.
	std::unique_ptr<st_graphics_context> graphics = st_graphics_context::create(api, window.get());

	// Create resources shared by many systems of the application.
	create_global_resources(graphics.get());

	// Create the shader manager, loading all the shaders.
	std::unique_ptr<st_shader_manager> shader_manager =
		std::make_unique<st_shader_manager>(graphics.get());

	// Create objects for phases of the frame: sim, physics, and output.
	std::unique_ptr<st_sim> sim = std::make_unique<st_sim>();
	std::unique_ptr<st_physics_world> world = std::make_unique<st_physics_world>();
	std::unique_ptr<st_output> output = std::make_unique<st_output>(window.get(), graphics.get());
	std::unique_ptr<st_scene> scene = std::make_unique<st_scene>();

	// Create camera.
	std::unique_ptr<st_camera> camera = std::make_unique<st_camera>(st_vec3f({ 0.0f, 1.8f, 3.5f }), window->get_width(), window->get_height());
	camera->set_yaw(180.0f);
	camera->set_pitch(20.0f);

	// Create the default font. Not a smart pointer because it's a global and will
	// destruct out of order with other systems it depends on.
	g_font = new st_font("VeraMono.ttf", 16.0f, 512, 512);

	// Create the imgui context.
	st_imgui::initialize(window.get(), graphics.get());

	scene->setup_lighting_test(sim.get());

	window->show();

	// TODO: HACK: Commit all loaded resources.
	graphics->end_frame();
	graphics->swap();

	// Main loop:
	while (true)
	{
		st_imgui::new_frame();

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

		st_imgui::update(&params, sim.get());
#if 1
		// Leave imgui off until an application-specific interface is spun up.
		ImGui::ShowDemoWindow();
#endif

		// Draw to screen.
		output->update(&params);
	}

	scene = nullptr;
	output = nullptr;

	st_imgui::shutdown();
	st_job::shutdown();

	destroy_global_resources();

	delete g_font;

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
