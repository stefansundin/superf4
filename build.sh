#!/bin/bash -ex
# sudo apt-get install gcc-mingw-w64 nsis

if [[ "$1" == "release" ]]; then
  mkdir -p bin/32 bin/64
  cp SuperF4.ini bin/32/
  cp SuperF4.ini bin/64/

  i686-w64-mingw32-windres -o superf4.o include/superf4.rc
  i686-w64-mingw32-gcc -o bin/32/SuperF4.exe superf4.c superf4.o -mwindows -lshlwapi -lpsapi -O2 -s

  x86_64-w64-mingw32-windres -o superf4.o include/superf4.rc
  x86_64-w64-mingw32-gcc -o bin/64/SuperF4.exe superf4.c superf4.o -mwindows -lshlwapi -lpsapi -O2 -s

  makensis -V2 -Dx64 installer.nsi
else
  x86_64-w64-mingw32-windres -o superf4.o include/superf4.rc
  x86_64-w64-mingw32-gcc -o SuperF4.exe superf4.c superf4.o -mwindows -lshlwapi -lpsapi -O2 -g -DDEBUG
fi
