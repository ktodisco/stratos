import os
import subprocess
import sys

def convert_hlsl_to_glsl(hlsl_file, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	vertex_file = basename + "_vert.glsl"
	fragment_file = basename + "_frag.glsl"
	
	print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
	
	# Run conversions.
	subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, vertex_file), "--entry", "vs_main", "--stage", "vs", "--target", "glsl"])
	subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, fragment_file), "--entry", "ps_main", "--stage", "ps", "--target", "glsl"])
		
def convert_hlsl_to_spirv(hlsl_file, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	vertex_file = basename + "_vert.spirv"
	fragment_file = basename + "_frag.spirv"
	
	print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
	
	# Run conversions.
	subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, vertex_file), "--entry", "vs_main", "--stage", "vs", "--target", "spirv"])
	subprocess.call([converter, "--input", hlsl_file, "--output", os.path.join(output_dir, fragment_file), "--entry", "ps_main", "--stage", "ps", "--target", "spirv"])

def build_sm6(hlsl_file, output_dir):
	converter = os.path.join("..", "DirectXShaderCompiler", "dxc.exe")
	
	basename = os.path.splitext(os.path.basename(hlsl_file))[0]
	vertex_file = basename + "_vert.cso"
	fragment_file = basename + "_frag.cso"
	
	print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
	
	# Run conversions.
	subprocess.call([converter, "-E", "vs_main", "-T", "vs_6_0", "-Fo", os.path.join(output_dir, vertex_file), hlsl_file])
	subprocess.call([converter, "-E", "ps_main", "-T", "ps_6_0", "-Fo", os.path.join(output_dir, fragment_file), hlsl_file])

if __name__ == "__main__":
	usage = [__file__, "<input file>", "<target> (glsl|spirv|dxil)", "<output director>"]
	
	if not len(sys.argv) == len(usage):
		print("Usage: " + ' '.join(usage))
		sys.exit(1)
		
	input_file = os.path.normpath(sys.argv[1])
	target_type = sys.argv[2]
	output_dir = os.path.normpath(sys.argv[3])
	
	# Ensure this directory exists.
	if not os.path.exists(output_dir):
		os.makedirs(output_dir)
		
	if target_type == 'glsl':
		convert_hlsl_to_glsl(input_file, output_dir)
	if target_type == 'spirv':
		convert_hlsl_to_spirv(input_file, output_dir)
	if target_type == 'dxil':
		build_sm6(input_file, output_dir)
