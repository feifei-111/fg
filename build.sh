export SRC_PATH=./src
clang++ -c $SRC_PATH/fg_gl/gl.cc -o gl.o  -std=c++11 -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I./third_party -I.src/ -I./include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib
clang++ -c $SRC_PATH/fg_utils/time.cc -o time.o  -std=c++11 -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I./third_party -I.src/ -I./include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib
clang++ -c $SRC_PATH/fg_model/model.cc -o model.o -std=c++11 -lassimp -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -I./third_party -I.src/ -I./include -I/opt/homebrew/Cellar/glfw/3.4/include -L/opt/homebrew/Cellar/glfw/3.4/lib -I./third_party/assimp/include -L./third_party/assimp/bin
ar rcs libfg.a gl.o time.o model.o
rm -rf *.o

