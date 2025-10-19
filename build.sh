if [ "$1" = "sub" ]; then
    bash submodules.sh
fi

rm -rf build
cmake -B build .
cmake --build build
