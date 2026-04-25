import os
import sys
import shutil
import platform
import subprocess

script_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.join(script_dir, "build")
install_dir = os.path.join(script_dir, "install")
executable = os.path.join(script_dir, "fg_example")

if len(sys.argv) < 2:
    print("Usage: python build.py <CMAKE_PREFIX_PATH>")
    sys.exit(1)

prefix_path = sys.argv[1]

# Clean
if platform.system() == "Windows":
    if os.path.exists(executable + ".exe"):
        os.remove(executable + ".exe")
else:
    if os.path.exists(executable):
        os.remove(executable)

if os.path.exists(build_dir):
    shutil.rmtree(build_dir)

# Configure & Build
subprocess.run(
    ["cmake", ".", "-B", "build", f"-DCMAKE_PREFIX_PATH={prefix_path}"],
    cwd=script_dir, check=True)

subprocess.run(
    ["cmake", "--build", "build", "--target", "install"],
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
