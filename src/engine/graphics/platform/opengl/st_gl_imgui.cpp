/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/opengl/st_gl_imgui.h>

#if defined(ST_GRAPHICS_API_OPENGL)

#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>

void st_gl_imgui::initialize(
	const st_window* window,
	const st_render_context* context)
{
	//ImGui_ImplOpenGL3_Init("#version 400");
}

void st_gl_imgui::shutdown()
{
	//ImGui_ImplOpenGL3_Shutdown();
}

void st_gl_imgui::new_frame()
{
	//ImGui_ImplOpenGL3_NewFrame();
}

void st_gl_imgui::draw()
{
	//ImGui::Render();
	//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif
