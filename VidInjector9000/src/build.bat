@echo off
set "input=%1"
set "output=%~n1"
echo compiling "%output%.exe"
rmdir obj\Debug\ /s /q
del "%output%.exe"
mkdir obj\Debug
g++ -Wall -c -g %input% -o obj\Debug\%output%.o
g++ -static -static-libgcc -static-libstdc++ -o "%output%.exe" obj\Debug\%output%.o
::g++ -g %input% -o "%output%.exe"
pause