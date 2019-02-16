#!/bin/bash -ex
# sudo apt-get install gcc-mingw-w64
x86_64-w64-mingw32-windres -o superf4.o include/superf4.rc
x86_64-w64-mingw32-gcc -o SuperF4.exe superf4.c superf4.o -mwindows -lshlwapi -lpsapi -O2 -s
