
taskkill /IM SuperF4.exe

gcc -c keyhook.c
gcc -shared -o keyhook.dll keyhook.o

windres -o resources.o resources.rc
gcc -o SuperF4 superf4.c resources.o -mwindows -lshlwapi

strip SuperF4.exe
strip keyhook.dll

upx --best -qq SuperF4.exe
upx --best -qq keyhook.dll
