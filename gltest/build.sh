export SRC_PATH=../src


clang++ mac_main.cc render.cc $SRC_PATH/fg_gl/gl.cc $SRC_PATH/fg_utils/time.cc -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I$SRC_PATH/third_party -I$SRC_PATH/ -I../include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib

