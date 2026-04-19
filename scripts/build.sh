#!/bin/bash
git submodule update --init --recursive
cmake -B build . -DUSE_VLOG=ON -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build build --config Release
