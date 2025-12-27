# build fg and submodules with script:
```sh
# if you not 
INSTALL_PATH=""
bash scripts/submodules.sh install ${INSTALL_PATH}
bash scripts/build.sh install ${INSTALL_PATH}
```

# build fg with command
```sh
cmake -B build .
cmake --build build -DUSE_VLOG=OFF -DVERBOSE=OFF
```

# Example
There is an example for libfg usage
- build and install
- create a link which can be moved and launch program in any directory
```sh
cd example
bash build.sh ${YOUR_INSTALL_PATH}
./fg_example
```