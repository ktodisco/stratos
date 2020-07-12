import os
import subprocess
import sys

def convert_hlsl_to_glsl(shader_dir, hlsl_files, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	for hlsl_file in hlsl_files:
		basename = os.path.splitext(hlsl_file)[0]
		vertex_file = basename + "_vert.glsl"
		fragment_file = basename + "_frag.glsl"
		
		print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
		
		# Run conversions.
		subprocess.call([converter, "--input", os.path.join(shader_dir, hlsl_file), "--output", os.path.join(output_dir, vertex_file), "--entry", "vs_main", "--stage", "vs", "--target", "glsl"])
		subprocess.call([converter, "--input", os.path.join(shader_dir, hlsl_file), "--output", os.path.join(output_dir, fragment_file), "--entry", "ps_main", "--stage", "ps", "--target", "glsl"])
		
def convert_hlsl_to_spirv(shader_dir, hlsl_files, output_dir):
	converter = os.path.join("..", "ShaderConductor", "Bin", "Release", "ShaderConductorCmd.exe")
	
	for hlsl_file in hlsl_files:
		basename = os.path.splitext(hlsl_file)[0]
		vertex_file = basename + "_vert.spirv"
		fragment_file = basename + "_frag.spirv"
		
		print("Converting %s to %s and %s." % (hlsl_file, vertex_file, fragment_file))
		
		# Run conversions.
		subprocess.call([converter, "--input", os.path.join(shader_dir, hlsl_file), "--output", os.path.join(output_dir, vertex_file), "--entry", "vs_main", "--stage", "vs", "--target", "spirv"])
		subprocess.call([converter, "--input", os.path.join(shader_dir, hlsl_file), "--output", os.path.join(output_dir, fragment_file), "--entry", "ps_main", "--stage", "ps", "--target", "spirv"])

if __name__ == "__main__":
	usage = [__file__, "<directory>", "<target> (glsl|spirv)", "<output directory>"]
	
	if not len(sys.argv) == len(usage):
		print("Usage: " + ' '.join(usage))
		sys.exit(1)
		
	shader_directory = os.path.normpath(sys.argv[1])
	target_type = sys.argv[2]
	output_directory = os.path.normpath(sys.argv[3])
	
	for (dirpath, dirnames, filenames) in os.walk(shader_directory):
		# Collect all .hlsl files.
		hlsl_files = [file for file in filenames if file.endswith(".hlsl")]
		
		if target_type == 'glsl':
			convert_hlsl_to_glsl(shader_directory, hlsl_files, output_directory)
		if target_type == 'spirv':
			convert_hlsl_to_spirv(shader_directory, hlsl_files, output_directory)
