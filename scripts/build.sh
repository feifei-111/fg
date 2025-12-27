#!/bin/bash
# ============================================================
SCRIPT_DIR=$(dirname $0)
source $SCRIPT_DIR/tools.sh
parse_install_args "$@"

# ============================================================
cmake -B build . -DUSE_VLOG=ON \
    -DCMAKE_BUILD_TYPE=Release \
    $CMAKE_INSTALL_PREFIX_STR \
    $CMAKE_PREFIX_STR
cmake --build build --config Release $CMAKE_TARGET_STR
