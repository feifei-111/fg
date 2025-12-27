#!/bin/bash

# ============================================================
git submodule update --init --recursive

# ============================================================
SCRIPT_DIR=$(dirname $0)
source $SCRIPT_DIR/tools.sh
parse_install_args "$@"

# ============================================================
# 其实 glm 不需要编译，不过它确实带了一个 cmake，
# 可以提前把一些模版特化编译成 lib，这很快
# 我们也不用这个 lib，总之，可以获取它的 include 就行了
cd third_party/glm
cmake . -B build \
    -DGLM_BUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    $CMAKE_INSTALL_PREFIX_STR
# glm install 不用 --target install，而是 -- install

if [ "${CMAKE_TARGET_STR}" != "" ]; then
    cmake --build build --config Release  -- install
else
    cmake --build build --config Release
fi

# ============================================================
cd ../assimp
cmake . -B build \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    $CMAKE_INSTALL_PREFIX_STR
cmake --build build --config Release $CMAKE_TARGET_STR

# ============================================================
cd ../glog
cmake -S . -B build \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_BUILD_TYPE=Release \
    $CMAKE_INSTALL_PREFIX_STR
cmake --build build --config Release $CMAKE_TARGET_STR

