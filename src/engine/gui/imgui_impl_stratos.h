// dear imgui: Renderer for Stratos
// This needs to be used along with a Platform Binding (e.g. Win32)

#pragma once

enum e_st_format;
struct ID3D12Device;
struct ID3D12GraphicsCommandList;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

// cmd_list is the command list that the implementation will use to render imgui draw lists.
// Before calling the render function, caller must prepare cmd_list by resetting it and setting the appropriate 
// render target and descriptor heap that contains font_srv_cpu_desc_handle/font_srv_gpu_desc_handle.
// font_srv_cpu_desc_handle and font_srv_gpu_desc_handle are handles to a single SRV descriptor to use for the internal font texture.
IMGUI_IMPL_API bool     ImGui_ImplStratos_Init(int num_frames_in_flight, e_st_format rtv_format, struct st_render_pass* pass);
IMGUI_IMPL_API void     ImGui_ImplStratos_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplStratos_NewFrame(class st_graphics_context* ctx);
IMGUI_IMPL_API void     ImGui_ImplStratos_RenderDrawData(ImDrawData* draw_data, class st_graphics_context* ctx);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_IMPL_API void     ImGui_ImplStratos_InvalidateDeviceObjects();
IMGUI_IMPL_API bool     ImGui_ImplStratos_CreateDeviceObjects(class st_graphics_context* ctx);