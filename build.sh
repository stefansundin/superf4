#!/bin/bash -ex
mkdir -p bin
x86_64-w64-mingw32-windres -o superf4.o include/superf4.rc
x86_64-w64-mingw32-gcc -o "bin/SuperF4.exe" superf4.c superf4.o -mwindows -lshlwapi -O2 -s
