/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <gui/st_gui.h>

#include <graphics/st_graphics.h>
#include <graphics/st_render_context.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>
#include <graphics/platform/dx12/st_dx12_render_context.h>

#include <system/st_window.h>

#include <examples/imgui_impl_dx12.h>
#include <examples/imgui_impl_win32.h>

void st_gui::initialize(
	const st_window* window,
	const st_render_context* context)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(window->get_window_handle());

	// TODO: Need to conditionally compiled based on target API.
	const st_dx12_render_context* dx12_context = reinterpret_cast<const st_dx12_render_context*>(context);
	ImGui_ImplDX12_Init(
		dx12_context->get_device(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		dx12_context->get_gui_heap()->get_handle_cpu(0),
		dx12_context->get_gui_heap()->get_handle_gpu(0));

	ImGui::StyleColorsDark();
}

void st_gui::new_frame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void st_gui::draw()
{
	// Total hack.
	ID3D12DescriptorHeap* heaps[] = { st_dx12_render_context::get()->get_gui_heap()->get() };
	st_dx12_render_context::get()->get_command_list()->SetDescriptorHeaps(1, heaps);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(
		ImGui::GetDrawData(),
		st_dx12_render_context::get()->get_command_list());
}
