@echo off
setlocal ENABLEDELAYEDEXPANSION
set "bin=VidInjector9001.exe"
set "cpp=c++17"
rmdir obj\Debug\ /s /q
del !bin!
mkdir obj\Debug

set files=
set files2=
for %%i in (*.c*) do (
	set "files=!files! obj\Debug\%%~ni.o"
	echo %%i
	g++ -Wall -c -g %%i -std=!cpp! -o obj\Debug\%%~ni.o
)
g++ -static -static-libgcc -static-libstdc++ -o "!bin!"%files%
pause