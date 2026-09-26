#!/usr/bin/env bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=TC-mingw.cmake 
cmake --build build --config Release
echo finished
