@echo off

set "BUILD_TYPE=Release"

rmdir /S /Q "out"
mkdir out
mkdir out/VidInjector9000
cd "VidInjector9002.5"

echo build VidInjector9002.5...
title build VidInjector9002.5...

cmake -B build -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%
cmake --build build --config %BUILD_TYPE% --parallel 4
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

mkdir "build/%BUILD_TYPE%/VidInjector9000Resources"
robocopy "VidInjector9000Resources/image" "build/%BUILD_TYPE%/VidInjector9000Resources/image" /e

cd "../VidInjector9002-CLI"

echo build VidInjector9002-CLI...
title build VidInjector9002-CLI...

cmake -B build -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%
cmake --build build --config %BUILD_TYPE% --parallel 4
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

robocopy "build/%BUILD_TYPE%" "../VidInjector9002.5/build/%BUILD_TYPE%/VidInjector9000Resources" VidInjector9002-CLI.exe

cd "../VidInjector9002.5/build/%BUILD_TYPE%"

echo copy out binary...
title copy out binary...

for /f "delims=" %%i in ('dir /b') do (
	robocopy . ../../../out/VidInjector9000 %%i /e
)
pause
