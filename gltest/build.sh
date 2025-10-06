export ROOT_PATH=../
clang++ mac_main.cc render.cc -lfg -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I$ROOT_PATH/third_party -I$ROOT_PATH/include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib -L$ROOT_PATH/
