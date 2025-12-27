# Build libfg and submodules with script:
```sh
# if you not 
export INSTALL_PATH="~/install"
bash scripts/submodules.sh install ${INSTALL_PATH}
bash scripts/build.sh install ${INSTALL_PATH}
```

# Build libfg with command
```sh
cmake -B build .
cmake --build build -DUSE_VLOG=ON -CMAKE_VERBOSE_MAKEFILE=OFF
```

# Build Example
There is an example for libfg usage
- build and install
- create a link which can be moved and launch program in any directory
- press `ESC` to quit
```sh
cd example
bash build.sh ${INSTALL_PATH}
./fg_example
```