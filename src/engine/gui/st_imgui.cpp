/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_imgui.h>

#include <graphics/st_graphics_context.h>
#include <graphics/st_render_texture.h>

#include <gui/imgui_impl_stratos.h>

#include <system/st_window.h>

#include <examples/imgui_impl_win32.h>

std::unique_ptr<st_render_pass> st_imgui::_render_pass = nullptr;
st_graphics_context* st_imgui::_context = nullptr;
bool st_imgui::_open = true;

void st_imgui::initialize(
	const st_window* window,
	st_graphics_context* context)
{
	_context = context;
	st_target_desc target[] =
	{
		{ context->get_present_target(), e_st_load_op::load, e_st_store_op::store }
	};
	_render_pass = context->create_render_pass(1, target, nullptr);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(window->get_window_handle());
	ImGui_ImplStratos_Init(2, st_format_r8g8b8a8_unorm, _render_pass.get());

	ImGui::StyleColorsDark();
}

void st_imgui::shutdown()
{
	ImGui_ImplStratos_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	_render_pass = nullptr;
	_context = nullptr;
}

void st_imgui::update()
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

	ImGui::End();
}

void st_imgui::new_frame()
{
	ImGui_ImplStratos_NewFrame(_context);
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void st_imgui::draw()
{
	ImGui::Render();

	_context->begin_render_pass(_render_pass.get(), nullptr, 0);
	ImGui_ImplStratos_RenderDrawData(ImGui::GetDrawData(), _context);
	_context->end_render_pass(_render_pass.get());
}
