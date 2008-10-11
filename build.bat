
taskkill /IM SuperF4.exe

gcc -c hooks.c
gcc -shared -o hooks.dll hooks.o

windres -o resources.o resources.rc
gcc -o SuperF4 superf4.c resources.o -mwindows -lshlwapi

strip SuperF4.exe
strip hooks.dll

upx --best -qq SuperF4.exe
upx --best -qq hooks.dll
