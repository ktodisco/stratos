/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/platform/dx12/st_dx12_imgui.h>

#include <graphics/platform/dx12/st_dx12_descriptor_heap.h>
#include <graphics/platform/dx12/st_dx12_graphics_context.h>

#include <system/st_window.h>

#if 0

#include <imgui.h>
#include <examples/imgui_impl_dx12.h>

void st_dx12_imgui::initialize(
	const class st_window* window,
	const class st_graphics_context* context)
{
	const st_dx12_graphics_context* dx12_context = reinterpret_cast<const st_dx12_graphics_context*>(context);
	ImGui_ImplDX12_Init(
		dx12_context->get_device(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		dx12_context->get_gui_heap()->get_handle_cpu(0),
		dx12_context->get_gui_heap()->get_handle_gpu(0));
}

void st_dx12_imgui::shutdown()
{
	ImGui_ImplDX12_Shutdown();
}

void st_dx12_imgui::new_frame()
{
	ImGui_ImplDX12_NewFrame();
}

void st_dx12_imgui::draw()
{
	// Total hack.
	ID3D12DescriptorHeap* heaps[] = { st_dx12_graphics_context::get()->get_gui_heap()->get() };
	st_dx12_graphics_context::get()->get_command_list()->SetDescriptorHeaps(1, heaps);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(
		ImGui::GetDrawData(),
		st_dx12_graphics_context::get()->get_command_list());
}

#endif
