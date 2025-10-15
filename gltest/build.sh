export ROOT_PATH=..
export RENDER_PATH=$1
export USE_GLOG=-DUSE_GLOG
clang++ mac_main.cc $RENDER_PATH/render.cc -std=c++14 -lassimp -lfg -lglfw3 -lglog \
    $USE_GLOG -DGLOG_USE_GLOG_EXPORT=ON\
    -framework Cocoa -framework OpenGL -framework IOKit \
    -I$RENDER_PATH -I$ROOT_PATH/third_party -I$ROOT_PATH/include \
    -L$ROOT_PATH \
    -I/opt/homebrew/Cellar/glfw/3.4/include \
    -L/opt/homebrew/Cellar/glfw/3.4/lib \
    -I$ROOT_PATH/third_party/assimp/include \
    -L$ROOT_PATH/third_party/assimp/bin \
    -I$ROOT_PATH/third_party/glog/src \
    -I$ROOT_PATH/third_party/glog/build \
    -L$ROOT_PATH/third_party/glog/build \
    -Wl,-rpath,${ROOT_PATH}/third_party/assimp/bin \
    -Wl,-rpath,${ROOT_PATH}/third_party/glog/build

xattr -d com.apple.quarantine $ROOT_PATH/third_party/assimp/bin/libassimp.*.dylib
xattr -d com.apple.quarantine $ROOT_PATH/third_party/glog/build/libglog*.dylib