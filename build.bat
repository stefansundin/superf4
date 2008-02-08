
gcc -c keyhook.c
gcc -shared -o keyhook.dll keyhook.o

windres -o resources.o resources.rc
gcc -o SuperF4 superf4.c resources.o -mwindows

strip SuperF4.exe
strip keyhook.dll

upx --best -q SuperF4.exe
upx --best -q keyhook.dll
