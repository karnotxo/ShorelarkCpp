#!/usr/bin/env python3

import json
import os
import glob
import sys
import platform

def main():
    workspace_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
    
    # Use absolute paths and handle Windows paths correctly
    include_dirs = [
        "-I" + os.path.normpath(os.path.join(workspace_root, "libs/simulation/include")),
        "-I" + os.path.normpath(os.path.join(workspace_root, "libs/neural_network/include")),
        "-I" + os.path.normpath(os.path.join(workspace_root, "libs/optimizer/include")),
        "-I" + os.path.normpath(os.path.join(workspace_root, "libs/genetic_algorithm/include")),
        "-I" + os.path.normpath(os.path.join(workspace_root, "libs/external/include")),
    ]
    
    # Convert all backslashes to forward slashes for clang compatibility
    include_dirs = [path.replace("\\", "/") for path in include_dirs]
    
    # Add standard includes appropriate for platform
    if platform.system() == "Windows":
        msvc_includes = [
            "-IC:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.29.30133/include",
            "-IC:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/ucrt",
            "-IC:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um",
            "-IC:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/shared",
        ]
        include_dirs.extend(msvc_includes)
    
    compiler_flags = [
        "-std=c++17",
        "-Wall",
        "-Wextra",
    ] + include_dirs
    
    compile_commands = []
    
    # Find all C++ source files (both .cc and .h files)
    for ext in ['cpp', 'cc', 'cxx', 'h', 'hpp', 'hxx']:
        for source_file in glob.glob(f"{workspace_root}/**/*.{ext}", recursive=True):
            rel_path = os.path.relpath(source_file, workspace_root)
            # Skip files in build directory
            if rel_path.startswith(('build/', 'out/', '.git/')):
                continue
                
            # Convert path to use forward slashes for clang
            source_file_path = source_file.replace("\\", "/")
            
            compile_commands.append({
                "directory": workspace_root.replace("\\", "/"),
                "command": f"clang++ {' '.join(compiler_flags)} -c {source_file_path} -o {source_file_path}.o",
                "file": source_file_path
            })
    
    # Write the compilation database
    output_file = os.path.join(workspace_root, "compile_commands.json")
    with open(output_file, 'w') as f:
        json.dump(compile_commands, f, indent=2)
    
    # Create symlink if on non-Windows
    if platform.system() != "Windows" and not os.path.exists(os.path.join(workspace_root, "build/compile_commands.json")):
        try:
            os.makedirs(os.path.join(workspace_root, "build"), exist_ok=True)
            os.symlink(
                os.path.join(workspace_root, "compile_commands.json"),
                os.path.join(workspace_root, "build/compile_commands.json")
            )
        except Exception as e:
            print(f"Warning: Could not create symlink: {e}")
    
    print(f"Generated compile_commands.json with {len(compile_commands)} entries at {output_file}")

if __name__ == "__main__":
    main()
