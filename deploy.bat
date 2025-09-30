@echo off

set "BUILD_TYPE=Release"

rmdir /S /Q "out"
mkdir "out"
cd "VidInjector9002.5"

echo build VidInjector9002.5...
title build VidInjector9002.5...

cmake -B build -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%
cmake --build build --config %BUILD_TYPE% --parallel 4
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

set "ZIP=windows.zip"

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

echo zip binary...
title zip binary...

SETLOCAL EnableDelayedExpansion
for /f "delims=" %%i in ('dir /b') do (
	set "FILES=!FILES! %%i"
)
:: remove leading whitespace
for /f "tokens=*" %%i in ("%FILES%") do (
    set "FILES=%%i"
)

echo "%FILES%" -^> "../../../out/%ZIP%"

tar.exe -a -c -f "../../../out/%ZIP%" %FILES%
pause
