@echo off

:: For traditional MinGW, set prefix32 to empty string
:: For mingw-w32, set prefix32 to i686-w64-mingw32-
:: For mingw-w64, set prefix64 to x86_64-w64-mingw32-

set prefix32=i686-w64-mingw32-
set prefix64=x86_64-w64-mingw32-

taskkill /IM SuperF4.exe

if not exist bin. mkdir bin
if not exist "bin/x64". mkdir "bin/x64"

if "%1" == "all" (
  %prefix32%windres -o bin/superf4.o include/superf4.rc

  echo.
  echo Building binaries
  %prefix32%gcc -o "bin/SuperF4.exe" superf4.c bin/superf4.o -mwindows -lshlwapi -O2 -s
  if not exist "bin/SuperF4.exe". exit /b

  if "%2" == "x64" (
    %prefix64%windres -o bin/x64/superf4.o include/superf4.rc
    %prefix64%gcc -o "bin/x64/SuperF4.exe" superf4.c bin/x64/superf4.o -mwindows -lshlwapi -O2 -s
    if not exist "bin/x64/SuperF4.exe". exit /b
  )

  echo Building installer
  if "%2" == "x64" (
    makensis /V2 /Dx64 installer.nsi
  ) else (
    makensis /V2 installer.nsi
  )
) else if "%1" == "x64" (
  %prefix64%windres -o bin/x64/superf4.o include/superf4.rc
  %prefix64%gcc -o SuperF4.exe superf4.c bin/x64/superf4.o -mwindows -lshlwapi -g -DDEBUG
) else (
  %prefix32%windres -o bin/superf4.o include/superf4.rc
  %prefix32%gcc -o SuperF4.exe superf4.c bin/superf4.o -mwindows -lshlwapi -g -DDEBUG

  if "%1" == "run" (
    start SuperF4.exe
  )
)
