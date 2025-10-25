if [ "$1" = "sub" ]; then
    bash submodules.sh
fi

rm -rf build
cmake -B build . -DUSE_VLOG=ON
cmake --build build
