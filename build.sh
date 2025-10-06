export SRC_PATH=./src
clang++ -c $SRC_PATH/fg_gl/gl.cc -o gl.o -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I./third_party -I.src/ -I./include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib
clang++ -c $SRC_PATH/fg_utils/time.cc -o time.o -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I./third_party -I.src/ -I./include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib
ar rcs libfg.a gl.o time.o
rm -rf *.o

