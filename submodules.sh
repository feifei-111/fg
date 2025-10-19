git submodule update --init --recursive

# build third party
cd third_party/assimp
cmake CMakeLists.txt
cmake --build .

cd ../glog
cmake -S . -B build -DCMAKE_CXX_STANDARD=14
cmake --build build

cd ../..

sys=`uname`
if [ $sys = "Darwin" ]; then
    xattr -d com.apple.quarantine ./third_party/assimp/bin/libassimp.*.dylib
    xattr -d com.apple.quarantine ./third_party/glog/build/libglog*.dylib
fi