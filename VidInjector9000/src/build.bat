@echo off
set "input=%1"
set "output=%~n1"
echo compiling "%output%.exe"
rmdir obj\Debug\ /s /q
del "%output%.exe"
::del "%output%x86.exe"
mkdir obj\Debug
g++ -Wall -c -g %input% -std=c++17 -o obj\Debug\%output%.o
g++ -static -static-libgcc -static-libstdc++ -o "%output%.exe" obj\Debug\%output%.o

::echo Press any key to build x86. . .
::pause>NUL
::g++ -Wall -c -g %input% -std=c++17 -ansi -m32 -o obj\Debug\%output%x86.o
::g++ -static -static-libgcc -static-libstdc++ -o "%output%x86.exe" obj\Debug\%output%x86.o

::g++ -g %input% -o "%output%.exe"
pause