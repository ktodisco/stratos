/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_imgui.h>

#include <framework/st_camera.h>
#include <framework/st_frame_params.h>
#include <framework/st_sim.h>

#include <graphics/st_drawcall.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <gui/imgui_impl_stratos.h>

#include <math/st_mat4f.h>
#include <math/st_vec4f.h>

#include <system/st_window.h>

#include <examples/imgui_impl_win32.h>

st_graphics_context* st_imgui::_context = nullptr;
bool st_imgui::_open = true;
bool st_imgui::_axes_widget = false;

void st_imgui::initialize(
	const st_window* window,
	st_graphics_context* context,
	e_st_format rtv_format,
	st_render_pass* pass)
{
	_context = context;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(window->get_window_handle());
	ImGui_ImplStratos_Init(2, rtv_format, pass);

	ImGui::StyleColorsDark();
}

void st_imgui::shutdown()
{
	ImGui_ImplStratos_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	_context = nullptr;
}

void st_imgui::update(st_frame_params* params, st_sim* sim, st_camera* camera)
{
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Stratos Engine", &_open))
	{
		ImGui::End();
		return;
	}

	if (ImGui::CollapsingHeader("Application Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Configuration: "
#if defined(_DEBUG)
			"debug"
#elif defined(_PROFILE)
			"profile"
#elif defined(_RELEASE)
			"release"
#else
			"unknown"
#endif
#if defined(_DEVELOPMENT)
			" (optimized)"
#endif
		);

		std::string api("Unknown");
		switch (st_graphics_context::get()->get_api())
		{
		case e_st_graphics_api::dx12: api = "dx12"; break;
		case e_st_graphics_api::opengl: api = "opengl"; break;
		case e_st_graphics_api::vulkan: api = "vulkan"; break;
		}

		ImGui::Text("Graphics API: %s", api.c_str());
	}

	camera->debug();
	sim->debug();

	if (ImGui::CollapsingHeader("Utilities"))
	{
		ImGui::Checkbox("Show World Axes", &_axes_widget);
	}

	if (_axes_widget)
	{
		draw_axes_widget(params);
	}

	ImGui::End();
}

void st_imgui::new_frame()
{
	ImGui_ImplStratos_NewFrame(_context);
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void st_imgui::draw(const st_frame_params* params)
{
	ImGui::Render();
	ImGui_ImplStratos_RenderDrawData(ImGui::GetDrawData(), _context);
}

void st_imgui::draw_axes_widget(st_frame_params* params)
{
	st_mat4f vp = params->_view * params->_projection;
	st_vec4f aspect_correction =
	{
		float(params->_width) / float(params->_height),
		1.0f,
		1.0f,
		1.0f,
	};

	st_vec4f x_view = vp.transform(st_vec4f::x_vector()) * aspect_correction;
	st_vec4f y_view = vp.transform(st_vec4f::y_vector()) * aspect_correction;
	st_vec4f z_view = vp.transform(st_vec4f::z_vector()) * aspect_correction;

	const float k_line_length = 15.0f;
	const st_vec2f k_origin = { 40, 50 };

	st_dynamic_drawcall drawcall;

	// x-axis
	drawcall._positions.push_back({ k_origin.x, k_origin.y, 0 });
	drawcall._positions.push_back({ k_origin.x + (x_view.x * k_line_length), k_origin.y + (-x_view.y * k_line_length), 0 });
	drawcall._colors.push_back({ 1.0f, 0.0f, 0.0f });
	drawcall._colors.push_back({ 1.0f, 0.0f, 0.0f });
	drawcall._indices.push_back(0);
	drawcall._indices.push_back(1);

	// y-axis
	drawcall._positions.push_back({ k_origin.x, k_origin.y, 0 });
	drawcall._positions.push_back({ k_origin.x + (y_view.x * k_line_length), k_origin.y + (-y_view.y * k_line_length), 0 });
	drawcall._colors.push_back({ 0.0f, 1.0f, 0.0f });
	drawcall._colors.push_back({ 0.0f, 1.0f, 0.0f });
	drawcall._indices.push_back(2);
	drawcall._indices.push_back(3);

	// z-axis
	drawcall._positions.push_back({ k_origin.x, k_origin.y, 0 });
	drawcall._positions.push_back({ k_origin.x + (z_view.x * k_line_length), k_origin.y + (-z_view.y * k_line_length), 0 });
	drawcall._colors.push_back({ 0.0f, 0.0f, 1.0f });
	drawcall._colors.push_back({ 0.0f, 0.0f, 1.0f });
	drawcall._indices.push_back(4);
	drawcall._indices.push_back(5);

	drawcall._color = { 1.0f, 1.0f, 1.0f };
	drawcall._draw_mode = st_primitive_topology_lines;
	drawcall._transform.make_identity();

	params->_gui_drawcalls.push_back(drawcall);
}
