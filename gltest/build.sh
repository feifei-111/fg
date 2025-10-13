export ROOT_PATH=../
export RENDER_PATH=$1
clang++ mac_main.cc $RENDER_PATH/render.cc -std=c++11 -lfg -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I$RENDER_PATH -I$ROOT_PATH/third_party -I$ROOT_PATH/include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib -L$ROOT_PATH/ -I$ROOT_PATH/third_party/assimp/include -L$ROOT_PATH/third_party/assimp/bin
