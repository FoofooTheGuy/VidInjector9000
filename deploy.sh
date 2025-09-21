#! /bin/bash

rm -rf 'out'
mkdir 'out'
cd 'VidInjector9002.5'

echo 'build VidInjector9002.5...'

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

mkdir 'build/VidInjector9000Resources'
cp -r 'VidInjector9000Resources/image' 'build/VidInjector9000Resources/'

cd '../VidInjector9002-CLI'

echo 'build VidInjector9002-CLI...'

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4

cp 'build/VidInjector9002-CLI' '../VidInjector9002.5/build/VidInjector9000Resources'

cd '../VidInjector9002.5/build'

echo 'zip binary...'

zip -r '../../out/DISTRO.zip' $(find './' -maxdepth 1 -not -name '.' -not -name 'CMakeFiles' -not -name 'resources' -not -name 'wxWidgets-*' -not -name 'CMakeCache.txt' -not -name 'Makefile' -not -name 'cmake_install.cmake')
