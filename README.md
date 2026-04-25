# Build

## Build with script
```sh
# build only
python scripts/build.py

# build and install to custom path
python scripts/build.py --prefix ~/install --install

# enable VLOG
python scripts/build.py --prefix ~/install --install --vlog
```

## Build with command
```sh
cmake -B build . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

## Example
There is an example for libfg usage
- build and install
- create a link which can be moved and launch program in any directory
- press `ESC` to quit
```sh
cd example
python build.py --prefix ~/install
./fg_example
```
