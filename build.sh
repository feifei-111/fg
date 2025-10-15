if [ $1 = "sub" ]; then
    git submodule update --init --recursive

    # build third party
    cd third_party/assimp
    cmake CMakeLists.txt
    cmake --build .

    cd ../glog
    cmake -S . -B build -DCMAKE_CXX_STANDARD=14
    cmake --build build

    cd ../..
fi

export USE_GLOG=-DUSE_GLOG

# build fg
export SRC_PATH=./src
clang++ -c $SRC_PATH/fg_gl/gl.cc -o gl.o -std=c++11 \
    $USE_GLOG \
    -lglog \
    -framework Cocoa -framework OpenGL -framework IOKit \
    -I./third_party -I.src/ -I./include \
    -I./third_party/glog/src \
    -I./third_party/glog/build \
    -L./third_party/glog/build
clang++ -c $SRC_PATH/fg_utils/time.cc -o time.o -std=c++11  \
    $USE_GLOG \
    -lglog \
    -framework Cocoa -framework OpenGL -framework IOKit \
    -I./third_party -I.src/ -I./include \
    -I./third_party/glog/src \
    -I./third_party/glog/build \
    -L./third_party/glog/build
clang++ -c $SRC_PATH/fg_model/model.cc -o model.o -std=c++11 \
    $USE_GLOG \
    -lassimp -lglog \
    -framework Cocoa -framework OpenGL -framework IOKit \
    -I./third_party -I.src/ -I./include \
    -I./third_party/assimp/include \
    -L./third_party/assimp/bin \
    -I./third_party/glog/src \
    -I./third_party/glog/build \
    -L./third_party/glog/build

ar rcs libfg.a gl.o time.o model.o
rm -rf *.o

