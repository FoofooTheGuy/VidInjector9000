@echo off

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

pause