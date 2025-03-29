function build_shader(filename)
	filter { string.format("files:%s", filename) }
		buildmessage "Building shader file %{Filename}"
		buildcommands {
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) spirv vps $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) dxil vps $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) glsl vps $(OutDir)data/shaders",
		}
		buildoutputs {
			"$(OutDir)data/shaders/%(Filename)_vert.spirv", "$(OutDir)data/shaders/%(Filename)_frag.spirv",
			"$(OutDir)data/shaders/%(Filename)_vert.cso", "$(OutDir)data/shaders/%(Filename)_frag.cso",
			"$(OutDir)data/shaders/%(Filename)_vert.glsl", "$(OutDir)data/shaders/%(Filename)_frag.glsl",
		}
		
	filter {}
end

function build_compute_shader(filename)
	filter { string.format("files:%s", filename) }
		buildmessage "Building compute shader file %{Filename}"
		buildcommands {
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) spirv cs $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) dxil cs $(OutDir)data/shaders",
			"python $(ProjectDir)../scripts/build_shader.py %(FullPath) glsl cs $(OutDir)data/shaders",
		}
		buildoutputs {
			"$(OutDir)data/shaders/%(Filename)_comp.spirv",
			"$(OutDir)data/shaders/%(Filename)_comp.cso",
			"$(OutDir)data/shaders/%(Filename)_comp.glsl",
		}
	
	filter {}
end

project "data"
	kind "Utility"
	
	files {
		"animations/*.*",
		"models/*.*",
		"scripts/*.*",
		"shaders/*.hlsl",
		"shaders/*.hlsli",
		"textures/**.*",
		"xnormal_test/*.*",
	}
	
	build_shader("shaders/imgui.hlsl")
	build_shader("shaders/st_atmosphere_apply.hlsl")
	build_shader("shaders/st_atmosphere_sky_view.hlsl")
	build_shader("shaders/st_bloom_downsample.hlsl")
	build_shader("shaders/st_bloom_threshold.hlsl")
	build_shader("shaders/st_bloom_upsample.hlsl")
	build_shader("shaders/st_constant_color.hlsl")
	build_shader("shaders/st_deferred_light.hlsl")
	build_shader("shaders/st_display.hlsl")
	build_shader("shaders/st_font_simple.hlsl")
	build_shader("shaders/st_fullscreen.hlsl")
	build_shader("shaders/st_gaussian_blur_horizontal.hlsl")
	build_shader("shaders/st_gaussian_blur_vertical.hlsl")
	build_shader("shaders/st_gbuffer.hlsl")
	build_shader("shaders/st_parallax_occlusion.hlsl")
	build_shader("shaders/st_phong.hlsl")
	build_shader("shaders/st_shadow.hlsl")
	build_shader("shaders/st_smaa_edges.hlsl")
	build_shader("shaders/st_smaa_weights.hlsl")
	build_shader("shaders/st_smaa_blend.hlsl")
	build_shader("shaders/st_tonemap.hlsl")
	build_shader("shaders/st_unlit_texture.hlsl")
	
	build_compute_shader("shaders/st_atmosphere_transmission.hlsl")
	
	prebuildcommands {
		"{COPYDIR} %[./] %[%{!cfg.targetdir}/data]",
	}