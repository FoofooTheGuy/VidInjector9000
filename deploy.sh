#!/usr/bin/env bash

rm -rf 'out'
mkdir 'out'
cd 'VidInjector9002.5'

echo 'build VidInjector9002.5...'

cmake -B build -DCMAKE_BUILD_TYPE=Release
ret="$?"
if [[ "$ret" -ne 0 ]]; then
	exit "$ret"
fi
cmake --build build --config Release --parallel 4
ret="$?"
if [[ "$ret" -ne 0 ]]; then
	exit "$ret"
fi

cd 'build'

# get the path of the binary since on mac it is a package
BIN90025=$(find './' -maxdepth 1 -not -name '.' -not -name 'CMakeFiles' -not -name 'resources' -not -name 'wxWidgets-*' -not -name 'CMakeCache.txt' -not -name 'Makefile' -not -name 'cmake_install.cmake' -not -name 'VidInjector9000Resources')
MAC=false
ZIP='Linux.zip'

# on mac, the package is a dir
if [[ -d $BIN90025 ]]; then
	MAC=true
	ZIP='MacOS.zip'
fi

cd ..

if [[ $MAC = true ]]; then
	mkdir "build/${BIN90025}/Contents/MacOS/VidInjector9000Resources"
	cp -r 'VidInjector9000Resources/image' "build/${BIN90025}/Contents/MacOS/VidInjector9000Resources/"
else
	mkdir 'build/VidInjector9000Resources'
	cp -r 'VidInjector9000Resources/image' 'build/VidInjector9000Resources/'
fi

cd '../VidInjector9002-CLI'

echo 'build VidInjector9002-CLI...'

cmake -B build -DCMAKE_BUILD_TYPE=Release
ret="$?"
if [[ "$ret" -ne 0 ]]; then
	exit "$ret"
fi
cmake --build build --config Release --parallel 4
ret="$?"
if [[ "$ret" -ne 0 ]]; then
	exit "$ret"
fi

if [[ $MAC = true ]]; then
	cp 'build/VidInjector9002-CLI' '../VidInjector9002.5/build/$BIN90025/Contents/MacOS/VidInjector9000Resources'
else
	cp 'build/VidInjector9002-CLI' '../VidInjector9002.5/build/VidInjector9000Resources'
fi

cd '../VidInjector9002.5/build'

echo 'zip binary...'

zip -r "../../out/${ZIP}" $(find './' -maxdepth 1 -not -name '.' -not -name 'CMakeFiles' -not -name 'resources' -not -name 'wxWidgets-*' -not -name 'CMakeCache.txt' -not -name 'Makefile' -not -name 'cmake_install.cmake')

echo 'finished'
