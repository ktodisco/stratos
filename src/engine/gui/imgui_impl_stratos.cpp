// dear imgui: Renderer for Stratos
// This needs to be used along with a Platform Binding (e.g. Win32)

#include "imgui.h"
#include "imgui_impl_stratos.h"

#include <framework/st_global_resources.h>

#include <graphics/st_drawcall.h>
#include <graphics/st_graphics.h>
#include <graphics/st_graphics_context.h>
#include <graphics/st_pipeline_state_desc.h>
#include <graphics/st_shader_manager.h>
#include <graphics/geometry/st_vertex_attribute.h>

#include <math/st_mat4f.h>

struct imgui_cb
{
    st_mat4f projection;
};

static st_render_pass* g_render_pass = nullptr;
//static ID3D12RootSignature* g_pRootSignature = nullptr;
static std::unique_ptr<st_buffer> g_constant_buffer = nullptr;
static std::unique_ptr<st_resource_table> g_resource_table = nullptr;
static std::unique_ptr<st_vertex_format> g_vertex_format = nullptr;
static std::unique_ptr<st_pipeline> g_pipeline = nullptr;
static e_st_format g_rtv_format = st_format_unknown;
static std::unique_ptr<st_texture> g_font_texture = nullptr;
static std::unique_ptr<st_texture_view> g_font_texture_view = nullptr;

struct FrameResources
{
    std::unique_ptr<st_buffer> IB;
    std::unique_ptr<st_buffer> VB;
    int VertexBufferSize;
    int IndexBufferSize;
};
static FrameResources*  g_pFrameResources = nullptr;
static uint32_t         g_numFramesInFlight = 0;
static uint32_t         g_frameIndex = UINT_MAX;

struct VERTEX_CONSTANT_BUFFER
{
    float   mvp[4][4];
};

// Render function
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
void ImGui_ImplStratos_RenderDrawData(ImDrawData* draw_data, st_graphics_context* ctx)
{
    // FIXME: I'm assuming that this only gets called once per frame!
    // If not, we can't just re-allocate the IB or VB, we'll have to do a proper allocator.
    g_frameIndex = g_frameIndex + 1;
    FrameResources* frameResources = &g_pFrameResources[g_frameIndex % g_numFramesInFlight];
    st_buffer* g_pVB = frameResources->VB.get();
    st_buffer* g_pIB = frameResources->IB.get();
    int g_VertexBufferSize = frameResources->VertexBufferSize;
    int g_IndexBufferSize = frameResources->IndexBufferSize;

    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB = nullptr; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;

        st_buffer_desc desc;
        desc._count = g_VertexBufferSize;
        desc._element_size = sizeof(ImDrawVert);
        desc._usage = e_st_buffer_usage::vertex | e_st_buffer_usage::transfer_dest;
        frameResources->VB = ctx->create_buffer(desc);

        g_pVB = frameResources->VB.get();
        frameResources->VertexBufferSize = g_VertexBufferSize;
    }

    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB = nullptr; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;

        st_buffer_desc desc;
        desc._count = g_IndexBufferSize;
        desc._element_size = sizeof(ImDrawIdx);
        desc._usage = e_st_buffer_usage::index | e_st_buffer_usage::transfer_dest;
        frameResources->IB = ctx->create_buffer(desc);

        g_pIB = frameResources->IB.get();
        frameResources->IndexBufferSize = g_IndexBufferSize;
    }

    // Copy and convert all vertices into a single contiguous buffer
    size_t vtx_offset = 0;
    size_t idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        uint8_t* head = nullptr;
        ctx->map(g_pVB, 0, { 0, 0 }, (void**)&head);
        memcpy(head + vtx_offset, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        ctx->unmap(g_pVB, 0, { 0, 0 });

        ctx->map(g_pIB, 0, { 0, 0 }, (void**)&head);
        memcpy(head + idx_offset, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        ctx->unmap(g_pIB, 0, { 0, 0 });

        vtx_offset += cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        idx_offset += cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
    }

    // Setup viewport
    st_viewport vp;
    memset(&vp, 0, sizeof(st_viewport));
    vp._width = draw_data->DisplaySize.x;
    vp._height = draw_data->DisplaySize.y;
    vp._min_depth = 0.0f;
    vp._max_depth = 1.0f;
    vp._x = vp._y = 0.0f;
    ctx->set_viewport(vp);

    // Some backends require the pipeline be set before updating constants.
    ctx->set_pipeline(g_pipeline.get());

    // Setup orthographic projection matrix into our constant buffer
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
    {
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        st_mat4f mvp =
        {
            2.0f / (R - L), 0.0f, 0.0f, (R + L) / (L - R),
            0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
            0.0f, 0.0f, 0.5f, 0.5f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };

        ctx->update_buffer(g_constant_buffer.get(), mvp.data, 0, 1);
    }

    st_static_drawcall dc;
    dc._draw_mode = st_primitive_topology_triangles;
    dc._vertex_buffer = g_pVB;
    dc._index_buffer = g_pIB;

    // Setup render state
    ctx->set_blend_factor(0.0f, 0.0f, 0.0f, 0.0f);

    // Render command lists
    vtx_offset = 0;
    idx_offset = 0;
    ImVec2 pos = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                st_texture_view* view = static_cast<st_texture_view*>(pcmd->TextureId);
                ctx->update_textures(g_resource_table.get(), 1, &view);

                ctx->bind_resource_table(g_resource_table.get());

                ctx->set_scissor(
                    int(pcmd->ClipRect.x - pos.x),
                    int(pcmd->ClipRect.y - pos.y),
                    int(pcmd->ClipRect.z - pos.x),
                    int(pcmd->ClipRect.w - pos.y));

                dc._index_offset = idx_offset;
                dc._vertex_offset = vtx_offset;
                dc._index_count = pcmd->ElemCount;

                ctx->draw(dc);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

static void ImGui_ImplStratos_CreateFontsTexture(st_graphics_context* ctx)
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        ctx->begin_loading();

        st_texture_desc desc;
        desc._width = width;
        desc._height = height;
        desc._levels = 1;
        desc._format = st_format_r8g8b8a8_unorm;
        desc._usage = e_st_texture_usage::sampled;
        desc._initial_state = st_texture_state_pixel_shader_read;
        desc._data = pixels;
        g_font_texture = ctx->create_texture(desc);
        ctx->set_texture_name(g_font_texture.get(), "ImGui Font");
        ctx->end_loading();

        g_font_texture_view = ctx->create_texture_view(g_font_texture.get());
    }

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)g_font_texture_view.get();
}

bool ImGui_ImplStratos_CreateDeviceObjects(st_graphics_context* ctx)
{
    if (g_pipeline)
        ImGui_ImplStratos_InvalidateDeviceObjects();

    ImGui_ImplStratos_CreateFontsTexture(ctx);

    // Create the constants and resource table
    {
        st_buffer_desc desc;
        desc._count = 1;
        desc._element_size = sizeof(imgui_cb);
        desc._usage = e_st_buffer_usage::uniform;
        g_constant_buffer = ctx->create_buffer(desc);
        ctx->add_constant(g_constant_buffer.get(), "type_cb0", st_shader_constant_type_block);

        g_resource_table = ctx->create_resource_table();
        st_texture* textures[] = { g_font_texture.get() };
        st_sampler* samplers[] = { _global_resources->_trilinear_clamp_sampler.get() };
        ctx->set_textures(g_resource_table.get(), 1, textures, samplers);
        st_buffer* cbs[] = { g_constant_buffer.get() };
        ctx->set_constant_buffers(g_resource_table.get(), 1, cbs);
    }

    std::vector<st_vertex_attribute> attributes;
    attributes.reserve(3);
    attributes.push_back(st_vertex_attribute(st_vertex_attribute_position, st_format_r32g32_float, 0));
    attributes.push_back(st_vertex_attribute(st_vertex_attribute_uv, st_format_r32g32_float, 1));
    attributes.push_back(st_vertex_attribute(st_vertex_attribute_color, st_format_r8g8b8a8_unorm, 2));

    g_vertex_format = ctx->create_vertex_format(attributes.data(), 3);

    st_graphics_state_desc desc;
    desc._primitive_topology_type = st_primitive_topology_type_triangle;
    desc._sample_mask = UINT_MAX;
    desc._render_target_count = 1;
    desc._render_target_formats[0] = g_rtv_format;
    desc._sample_desc._count = 1;

    desc._shader = st_shader_manager::get()->get_shader(st_shader_imgui);
    desc._vertex_format = g_vertex_format.get();
    desc._pass = g_render_pass;

    // Create the blending setup
    desc._blend_desc._target_blend[0]._blend = true;
    desc._blend_desc._target_blend[0]._src_blend = st_blend_src_alpha;
    desc._blend_desc._target_blend[0]._dst_blend = st_blend_inv_src_alpha;
    desc._blend_desc._target_blend[0]._blend_op = st_blend_op_add;
    desc._blend_desc._target_blend[0]._src_blend_alpha = st_blend_inv_src_alpha;
    desc._blend_desc._target_blend[0]._dst_blend_alpha = st_blend_zero;
    desc._blend_desc._target_blend[0]._blend_op_alpha = st_blend_op_add;

    // Create the rasterizer state
    desc._rasterizer_desc._fill_mode = st_fill_mode_solid;
    desc._rasterizer_desc._cull_mode = st_cull_mode_none;
    desc._rasterizer_desc._winding_order_clockwise = true;

    // Create depth-stencil State
    desc._depth_stencil_desc._depth_enable = false;
    desc._depth_stencil_desc._stencil_write_mask = 0xf;
    desc._depth_stencil_desc._depth_compare = st_compare_func_always;
    desc._depth_stencil_desc._stencil_enable = false;

    desc._dynamic_scissor = true;

    g_pipeline = ctx->create_pipeline(desc);

    return true;
}

void ImGui_ImplStratos_InvalidateDeviceObjects()
{
    g_pipeline = nullptr;
    g_vertex_format = nullptr;
    g_resource_table = nullptr;
    g_constant_buffer = nullptr;
    g_font_texture_view = nullptr;
    g_font_texture = nullptr;

    for (uint32_t i = 0; i < g_numFramesInFlight; i++)
    {
        g_pFrameResources[i].IB = nullptr;
        g_pFrameResources[i].VB = nullptr;
    }
}

bool ImGui_ImplStratos_Init(int num_frames_in_flight, e_st_format rtv_format, st_render_pass* pass)
{
    g_rtv_format = rtv_format;
    g_render_pass = pass;
    g_pFrameResources = new FrameResources[num_frames_in_flight];
    g_numFramesInFlight = num_frames_in_flight;
    g_frameIndex = UINT_MAX;

    // Create buffers with a default size (they will later be grown as needed)
    for (int i = 0; i < num_frames_in_flight; i++)
    {
        g_pFrameResources[i].IB = nullptr;
        g_pFrameResources[i].VB = nullptr;
        g_pFrameResources[i].VertexBufferSize = 5000;
        g_pFrameResources[i].IndexBufferSize = 10000;
    }

    return true;
}

void ImGui_ImplStratos_Shutdown()
{
    ImGui_ImplStratos_InvalidateDeviceObjects();
    delete[] g_pFrameResources;
    g_render_pass = nullptr;
    g_pFrameResources = nullptr;
    g_numFramesInFlight = 0;
    g_frameIndex = UINT_MAX;
}

void ImGui_ImplStratos_NewFrame(st_graphics_context* ctx)
{
    if (!g_pipeline)
        ImGui_ImplStratos_CreateDeviceObjects(ctx);
}