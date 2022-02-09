@echo off
color 07
title VidInjector 9000 by Foofoo_the_guy

set ShortName=Title
set LongName=Title
set Publisher=Video
echo Welcome to VidInjector 9000!
echo.
echo We will start by selecting a base cia for the video injection.
pause
color 08
::from https://stackoverflow.com/a/61741362 and edited a bit so it only outputs one file, i will use this a lot later
set cmd=Add-Type -AssemblyName System.Windows.Forms;$f=new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory= [environment]::GetFolderPath('%~dp0');$f.Filter='CTR Importable Archives(*.cia)^|*.cia^';$f.Multiselect=$false;[void]$f.ShowDialog();if($f.Multiselect) {$f.FileNames}else{$f.FileName}
set pwshcmd=powershell -noprofile -command "&{%cmd%}"
for /f %%i in ('%pwshcmd%') do call :sum "%%i" ret

title VidInjector 9000 by Foofoo_the_guy

set "ciaName=%ret%"
echo Extracting "%ciaName%"...
echo.
cd Vidinector9000Resources\tools
::extraction scripts copied from ihaveamac
ctrtool.exe --content="%~dp0temp" "%ciaName%"
for %%0 in (%~dp0temp.0000*.*) do (
ctrtool.exe --exefsdir="%~dp0exefs" --romfsdir="%~dp0romfs" --exheader="%~dp0exheader.bin" "%~dp0temp.0000%%~x0"
del "%~dp0temp.0000%%~x0"
)
echo Done!

echo.
echo Importing and Extracting banner...
copy "%~dp0Vidinector9000Resources\files\banner.bin" "%~dp0exefs"
::extract banner
3dstool.exe -xvtf banner "%~dp0exefs\banner.bin" --banner-dir "%~dp0Banner"
echo Done!
color 07
::copy video to romfs
echo.
echo Next you will choose the .moflex video to inject.
pause

color 08
set cmd=Add-Type -AssemblyName System.Windows.Forms;$f=new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory= [environment]::GetFolderPath('%~dp0');$f.Filter='3DS Video Files(*.moflex)^|*.moflex^';$f.Multiselect=$false;[void]$f.ShowDialog();if($f.Multiselect) {$f.FileNames}else{$f.FileName}
set pwshcmd=powershell -noprofile -command "&{%cmd%}"
for /f %%i in ('%pwshcmd%') do call :sum "%%i" ret

title VidInjector 9000 by Foofoo_the_guy

set "moflexName=%ret%"
erase "%~dp0romfs\movie\movie.moflex"
copy "%moflexName%" "%~dp0romfs\movie"
::https://www.dostips.com/forum/viewtopic.php?f=3&t=409
For %%A in (%moflexName%) do (
    Set Name=%%~nxA
)
ren "%~dp0romfs\movie\%Name%" movie.moflex

color 07
::choose image and inject it
echo.
echo Next you will choose an image you want to use for the banner.
echo (The Image should be 200x120 for best results)
pause

color 08
set cmd=Add-Type -AssemblyName System.Windows.Forms;$f=new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory= [environment]::GetFolderPath('%~dp0');$f.Filter='200 x 120 Banner Images(*.png, *.jpg, *.jpeg, *.bmp)^|*.png;*.jpg;*.jpeg;*.bmp^';$f.Multiselect=$false;[void]$f.ShowDialog();if($f.Multiselect) {$f.FileNames}else{$f.FileName}
set pwshcmd=powershell -noprofile -command "&{%cmd%}"
for /f %%i in ('%pwshcmd%') do call :sum "%%i" ret

title VidInjector 9000 by Foofoo_the_guy

set "bannerName=%ret%"
"%~dp0Vidinector9000Resources\tools\imagemagick\magick.exe" "%bannerName%" -resize 200x120! -background black -compose Copy -gravity northwest -extent 256x128 "%~dp0Banner\COMMON0.png"

color 07
echo.
echo.
echo Now, you will have to use Ohana3DS to inject the banner.
echo Press continue, and Ohana3DS will open, then do these steps:
echo.
echo 1. Click on the anime girl
echo.
echo 2. Click on the "Textures" tab on the Left side
echo.
echo 3. Drag and drop "%~dp0Banner\banner0.bcmdl" onto the program
echo ^(or go to the textures tab and click "Open" on the bottommost tray and
echo locate that file^)
echo.
echo 4. Click "Import" on the bottommost tray
echo.
echo 5. Select "%~dp0Banner\COMMON0.png"
echo.
echo 6. Click Save
echo.
echo 7. Close Ohana3DS
echo.
pause

color 08
Ohana3DS.exe

::compile banner
echo.
echo Compiling new banner...
3dstool.exe -cvtf banner "%~dp0exefs\banner.bin" --banner-dir "%~dp0Banner"
echo Done!

color 07
::do smdh stuff
echo.
echo Next, you will choose an image to use as your icon
echo ^(The Image should be 48x48 for best results^)
pause

color 08
set cmd=Add-Type -AssemblyName System.Windows.Forms;$f=new-object Windows.Forms.OpenFileDialog;$f.InitialDirectory= [environment]::GetFolderPath('%~dp0');$f.Filter='48 x 48 Icon Images(*.png, *.jpg, *.jpeg, *.bmp)^|*.png;*.jpg;*.jpeg;*.bmp^';$f.Multiselect=$false;[void]$f.ShowDialog();if($f.Multiselect) {$f.FileNames}else{$f.FileName}
set pwshcmd=powershell -noprofile -command "&{%cmd%}"
for /f %%i in ('%pwshcmd%') do call :sum "%%i" ret

title VidInjector 9000 by Foofoo_the_guy

set "iconName=%ret%"
mkdir "%~dp0Icon"
"%~dp0Vidinector9000Resources\tools\imagemagick\magick.exe" convert "%iconName%" -resize 48x48! -background black -flatten "%~dp0Icon\Icon.png"
echo.
color 07
echo Enter the short name of your injection:
set /p "ShortName="
echo.
echo Enter the long name of your injection:
set /p "LongName="
echo.
echo Enter the Publisher of your injection:
set /p "Publisher="
echo.

color 08
::create icon.bin
echo Compiling icon.bin...
bannertool.exe makesmdh -i "%~dp0Icon\Icon.png" -s "%ShortName%" -l "%LongName%" -p "%Publisher%" -f visible,nosavebackups -o "%~dp0exefs/icon.bin"
echo Done!
echo.
echo Generating movie_title.csv...

setlocal EnableDelayedExpansion
::https://stackoverflow.com/a/35078127
rem Store the string in chr.tmp file
set /P "=%LongName%," < NUL > chr.tmp
rem Create zero.tmp file with the same number of Ascii zero characters
for %%a in (chr.tmp) do fsutil file createnew zero.tmp %%~Za > NUL
rem Compare both files with FC /B and get the differences
set "hex="
for /F "skip=1 tokens=2" %%a in ('fc /B chr.tmp zero.tmp') do set "hex=!hex!%%a"
del chr.tmp zero.tmp
::https://tanaikech.github.io/2017/12/30/splitting-string-by-n-characters-for-batch-file/
SET "STR=%hex%"
REM Split STR by N characters
SET "N=2"

:LOOP
SET "RES=%RES%!STR:~0,%N%!00"
SET "STR=!STR:~%N%!"
IF DEFINED STR GOTO LOOP
set output=%RES:~0,-1%0
setlocal DisableDelayedExpansion
::https://stackoverflow.com/a/41810676
>temp.bin echo(FFFE23004A0050002C00230045004E002C002300460052002C002300470045002C002300490054002C002300530050002C002300430048002C0023004B004F002C002300440055002C00230050004F002C002300520055002C002300540057000D000A00%output%%output%%output%%output%%output%%output%%output%%output%%output%%output%%output%%output%
certutil -f -decodehex temp.bin %~dp0romfs\movie\movie_title.csv >nul
del temp.bin
echo Done!

setlocal EnableDelayedExpansion
:enterTID
set hexTID=0
set decTID=0
set line=
color 07
echo.
echo Enter 5 hex integers for the ID of your cia ^(C0000 - EFFFF^) or
echo just type "0" for a random title ID.
echo ^(TID is ^in format 000400000XXXXX00 ^(that's hex^), the rest will auto fill^)
set /p "hexTID="
set hexTID=%hexTID:~0,5%
::https://stackoverflow.com/a/9232411
if not defined hexTID echo You must enter a value. Try again.&goto enterTID
set "test=!hexTID!"
for %%C in (0 1 2 3 4 5 6 7 8 9 A B C D E F) do if defined test set "test=!test:%%C=!"
if defined test echo Invalid input ^(%test%^). Try again.&goto enterTID
TIDthing.exe %hexTID%
for /f "usebackq eol= tokens=* delims= " %%a in (`findstr /n ^^^^ "output.txt"`) do (
    set line=%%a
    set "line=!line:*:=!"
)
set decTID=%line%
del output.txt
if "%decTID%" == "789760" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "844236" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "889374" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "892929" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "892930" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "892931" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "948826" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "966912" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "973200" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "983020" (
echo Oops, you ran into a blacklisted ID^!, try again.&goto enterTID
)else if "%decTID%" == "983021" (
echo Oops, you ran into a blacklisted ID^!, try again.
goto enterTID
)
::https://www.dostips.com/forum/viewtopic.php?t=2261
call cmd /c exit /b %decTID%
set hexTID=%=exitcode%
for /f "tokens=* delims=0" %%A in ("%hexTID%") do set "hexTID=%%A"&if not defined hexTID set "hexTID=0"
echo %hexTID% passed all checks^!
setlocal DisableDelayedExpansion
echo Building CIA...
cd %~dp0
mkdir "output"
::build cia! :D
%~dp0Vidinector9000Resources\tools\makerom.exe -f cia -o "output\%LongName% [000400000%hexTID%00].cia" -banner "exefs\banner.bin" -icon "exefs\icon.bin" -code "exefs\code.bin" -exheader "exheader.bin" -rsf %~dp0Vidinector9000Resources\files\template.rsf -DAPP_UNIQUE_ID=%decTID%

echo.
echo Injection Complete^! output CIA is is "%~dp0output\%LongName% [000400000%hexTID%00].cia"
echo.
echo This will clean out the folder of the listed files/folders^:
echo -romfs
echo -exefs
echo -Banner
echo -Icon
echo -exheader.bin
echo.
color 07
pause
color 08
rmdir romfs /s /q
rmdir exefs /s /q
rmdir Banner /s /q
rmdir Icon /s /q
del exheader.bin
color 07
echo.
echo Vidinjector9000 will now close.
echo.
pause
exit /B

::this is from https://stackoverflow.com/a/61741362 too
:sum [mud] [ret]
echo "%~1"
set FileName="%~1"
set ret=%FileName%
exit /B