build libfg with single cmd:
```sh
bash build.sh sub
```

build submodules
```sh
bash submodules.sh
```

build with cmake
```sh
cmake -B build .
cmake --build build -DUSE_VLOG=OFF -DVERBOSE=OFF
```