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
		copy "localization\%%f\info.txt" "build/%%f/SuperF4"
		copy "SuperF4.ini" "build/%%f/SuperF4"
		
		gcc -o "build/%%f/SuperF4/SuperF4.exe" superf4.c build/resources.o -mwindows -lshlwapi -lwininet -DL10N_FILE=\"localization/%%f/strings.h\"
		if exist "build/%%f/SuperF4/SuperF4.exe" (
			strip "build/%%f/SuperF4/SuperF4.exe"
			upx --best -qq "build/%%f/SuperF4/SuperF4.exe"
		)
	)
	
	@echo.
	echo Building installer
	makensis /V2 installer.nsi
) else (
	gcc -o SuperF4.exe superf4.c build/resources.o -mwindows -lshlwapi -lwininet
	
	if "%1" == "run" (
		start SuperF4.exe
	)
	if "%1" == "hide" (
		start SuperF4.exe -hide
	)
)
