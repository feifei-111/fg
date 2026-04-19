#!/usr/bin/env python3
import argparse
import os
import sys


def main():
    parser = argparse.ArgumentParser(description="Build fg library")
    parser.add_argument("--prefix", help="CMAKE_INSTALL_PREFIX and CMAKE_PREFIX_PATH")
    parser.add_argument("--install", action="store_true", help="install fg after build")
    parser.add_argument("--vlog", action="store_true", help="enable VLOG")
    args = parser.parse_args()

    cmake_args = "-DCMAKE_BUILD_TYPE=Release"
    if args.vlog:
        cmake_args += " -DUSE_VLOG=ON"
    if args.prefix:
        cmake_args += f" -DCMAKE_INSTALL_PREFIX={args.prefix}"
        cmake_args += f" -DCMAKE_PREFIX_PATH={args.prefix}"

    build_args = "--config Release"
    if args.install:
        build_args += " --target install"

    commands = [
        f"cmake -B build . {cmake_args}",
        f"cmake --build build {build_args}",
    ]

    for cmd in commands:
        print(f"=> {cmd}")
        ret = os.system(cmd)
        if ret != 0:
            sys.exit(ret >> 8)


if __name__ == "__main__":
    main()
