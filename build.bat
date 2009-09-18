@echo off

taskkill /IM SuperF4.exe

if not exist build (
	mkdir build
)

windres -o build\resources.o include\resources.rc

if "%1" == "all" (
	gcc -o build\ini.exe include\ini.c -lshlwapi -march=pentium2
	
	@echo.
	echo Building binaries
	if not exist "build\en-US\SuperF4" (
		mkdir "build\en-US\SuperF4"
	)
	gcc -o "build\en-US\SuperF4\SuperF4.exe" superf4.c build\resources.o -mwindows -lshlwapi -lwininet -march=pentium2 -O2
	if not exist "build\en-US\SuperF4\SuperF4.exe" (
		exit /b
	)
	strip "build\en-US\SuperF4\SuperF4.exe"
	
	for /D %%f in (localization/*) do (
		@echo.
		echo Putting together %%f
		if not %%f == en-US (
			if not exist "build\%%f\SuperF4" (
				mkdir "build\%%f\SuperF4"
			)
			copy "build\en-US\SuperF4\SuperF4.exe" "build\%%f\SuperF4"
		)
		copy "localization\%%f\info.txt" "build\%%f\SuperF4"
		copy SuperF4.ini "build\%%f\SuperF4"
		"build\ini.exe" "build\%%f\SuperF4\SuperF4.ini" SuperF4 Language %%f
	)
	
	@echo.
	echo Building installer
	makensis /V2 installer.nsi
) else (
	gcc -o SuperF4.exe superf4.c build\resources.o -mwindows -lshlwapi -lwininet -march=pentium2 -DDEBUG
	
	if "%1" == "run" (
		start SuperF4.exe
	)
	if "%1" == "hide" (
		start SuperF4.exe -hide
	)
)
