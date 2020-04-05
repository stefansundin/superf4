#!/bin/bash -ex
# sudo apt-get install gcc-mingw-w64

if [[ "$1" == "release" ]]; then
  mkdir -p bin/32 bin/64

  i686-w64-mingw32-windres -o xkill.o include/xkill.rc
  i686-w64-mingw32-gcc -o bin/32/xkill.exe xkill.c xkill.o -mwindows -O2 -s

  x86_64-w64-mingw32-windres -o xkill.o include/xkill.rc
  x86_64-w64-mingw32-gcc -o bin/64/xkill.exe xkill.c xkill.o -mwindows -O2 -s
else
  x86_64-w64-mingw32-windres -o xkill.o include/xkill.rc
  x86_64-w64-mingw32-gcc -o xkill.exe xkill.c xkill.o -mwindows -O2 -g -DDEBUG
fi
