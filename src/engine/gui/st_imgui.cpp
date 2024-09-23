/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_imgui.h>

#include <graphics/st_imgui_graphics.h>

#include <system/st_window.h>

#include <examples/imgui_impl_win32.h>

void st_imgui::initialize(
	const st_window* window,
	const st_graphics_context* context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(window->get_window_handle());
	st_imgui_graphics::initialize(window, context);

	ImGui::StyleColorsDark();
}

void st_imgui::shutdown()
{
	st_imgui_graphics::shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void st_imgui::new_frame()
{
	st_imgui_graphics::new_frame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void st_imgui::draw()
{
	st_imgui_graphics::draw();
}
