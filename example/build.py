#!/usr/bin/env python3
import argparse
import os
import shutil
import platform
import subprocess


def main():
    parser = argparse.ArgumentParser(description="Build fg example")
    parser.add_argument("--prefix", required=True, help="CMAKE_PREFIX_PATH (fg install path)")
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    build_dir = os.path.join(script_dir, "build")
    install_dir = os.path.join(script_dir, "install")
    executable = os.path.join(script_dir, "fg_example")

    # Clean
    if platform.system() == "Windows":
        exe = executable + ".exe"
        if os.path.exists(exe):
            os.remove(exe)
    else:
        if os.path.exists(executable):
            os.remove(executable)

    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    # Configure & Build
    subprocess.run(
        ["cmake", ".", "-B", "build",
         f"-DCMAKE_PREFIX_PATH={args.prefix}",
         "-DCMAKE_BUILD_TYPE=Release"],
        cwd=script_dir, check=True)

    subprocess.run(
        ["cmake", "--build", "build", "--config", "Release", "--target", "install"],
        cwd=script_dir, check=True)

    # Link / Copy executable
    installed_exe = os.path.join(install_dir, "fg_example")
    if platform.system() == "Windows":
        installed_exe += ".exe"
        executable += ".exe"
        shutil.copy2(installed_exe, executable)
    else:
        os.symlink(installed_exe, executable)

    print("Done.")


if __name__ == "__main__":
    main()
