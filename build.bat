@echo off

taskkill /IM SuperF4.exe

if not exist build (
	mkdir build
)

windres -o build/resources.o resources.rc

if "%1" == "all" (
	echo Building all
	
	for /D %%f in (localization/*) do (
		@echo.
		echo Building %%f
		if not exist "build/%%f/SuperF4" (
			mkdir "build\%%f\SuperF4"
		)
		copy "localization/%%f/info.txt" "build/%%f/SuperF4/info.txt"
		
		gcc -o "build/%%f/SuperF4/SuperF4.exe" superf4.c build/resources.o -mwindows -lshlwapi -DL10N_FILE=\"localization/%%f/strings.h\"
		if exist "build/%%f/SuperF4/SuperF4.exe" (
			strip "build/%%f/SuperF4/SuperF4.exe"
			upx --best -qq "build/%%f/SuperF4/SuperF4.exe"
		)
		
		gcc -c -o "build/%%f/hooks.o" hooks.c
		if exist "build/%%f/SuperF4/hooks.o" (
			gcc -shared -o "build/%%f/SuperF4/hooks.dll" "build/%%f/hooks.o"
			strip "build/%%f/SuperF4/hooks.dll"
			upx --best -qq "build/%%f/SuperF4/hooks.dll"
		)
	)
) else (
	gcc -o SuperF4.exe superf4.c build/resources.o -mwindows -lshlwapi
	gcc -c -o "build/hooks.o" hooks.c
	gcc -shared -o "hooks.dll" "build/hooks.o"
	
	if "%1" == "run" (
		start SuperF4.exe
	)
)
