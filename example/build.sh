rm fg_example
cmake . -B build -DCMAKE_PREFIX_PATH=$1
cmake --build build --target install
# link abs path
ln -s $PWD/install/fg_example