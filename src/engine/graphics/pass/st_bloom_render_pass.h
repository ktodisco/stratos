#pragma once

/*
** Stratos Rendering Engine
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include <graphics/pass/st_fullscreen_render_pass.h>

#include <array>
#include <memory>

class st_bloom_render_pass : public st_fullscreen_render_pass
{
public:
	st_bloom_render_pass(
		class st_render_texture* source_buffer,
		class st_render_texture* target_buffer);
	~st_bloom_render_pass();

	void render(class st_graphics_context* context, const struct st_frame_params* params);

private:
	std::unique_ptr<class st_render_texture> _threshold_target = nullptr;
	std::unique_ptr<class st_material> _threshold_material = nullptr;
	std::unique_ptr<struct st_render_pass> _threshold_pass = nullptr;
	std::unique_ptr<struct st_framebuffer> _threshold_framebuffer = nullptr;

	static const uint32_t k_num_downsamples = 4;
	std::array<std::unique_ptr<struct st_render_pass>, k_num_downsamples> _downsample_passes;
	std::array<std::unique_ptr<struct st_render_pass>, k_num_downsamples - 1> _upsample_passes;

	std::array<std::unique_ptr<struct st_framebuffer>, k_num_downsamples> _downsample_framebuffers;
	std::array<std::unique_ptr<struct st_framebuffer>, k_num_downsamples - 1> _upsample_framebuffers;

	std::array<std::unique_ptr<class st_material>, k_num_downsamples> _downsample_materials;
	std::array<std::unique_ptr<class st_material>, k_num_downsamples - 1> _upsample_materials;

	std::array<std::unique_ptr<class st_render_texture>, k_num_downsamples> _downsample_targets;
	std::array<std::unique_ptr<class st_render_texture>, k_num_downsamples> _upsample_targets;

	std::array<std::unique_ptr<class st_gaussian_blur_render_pass>, k_num_downsamples> _blur_passes;
};
