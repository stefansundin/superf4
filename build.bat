@echo off

:: For traditional MinGW, set prefix32 to empty string
:: For mingw-w32, set prefix32 to i686-w64-mingw32-
:: For mingw-w64, set prefix64 to x86_64-w64-mingw32-

set prefix32=i686-w64-mingw32-
set prefix64=x86_64-w64-mingw32-

taskkill /IM SuperF4.exe

if not exist build. mkdir build

%prefix32%windres -o build\superf4.o include\superf4.rc

if "%1" == "all" (
	%prefix32%gcc -o build\ini.exe include\ini.c -lshlwapi -march=pentium2
	
	@echo.
	echo Building binaries
	%prefix32%gcc -o "build\SuperF4.exe" superf4.c build\superf4.o -mwindows -lshlwapi -lwininet -march=pentium2 -O2 -s
	if not exist "build\SuperF4.exe". exit /b
	
	if "%2" == "x64" (
		%prefix64%windres -o build\superf4_x64.o include\superf4.rc
		if not exist "build\x64". mkdir "build\x64"
		%prefix64%gcc -o "build\x64\SuperF4.exe" superf4.c build\superf4_x64.o -mwindows -lshlwapi -lwininet -march=pentium2 -O2 -s
		if not exist "build\x64\SuperF4.exe". exit /b
	)
	
	for /D %%f in (localization/*) do (
		if not exist "localization\%%f\outdated.txt" (
			@echo.
			echo Putting together %%f
			if not exist "build\%%f\SuperF4". mkdir "build\%%f\SuperF4"
			copy "build\SuperF4.exe" "build\%%f\SuperF4"
			copy "localization\%%f\info.txt" "build\%%f\SuperF4"
			copy SuperF4.ini "build\%%f\SuperF4"
			"build\ini.exe" "build\%%f\SuperF4\SuperF4.ini" SuperF4 Language %%f
			if "%2" == "x64" (
				if not exist "build\x64\%%f\SuperF4". mkdir "build\x64\%%f\SuperF4"
				copy "build\x64\SuperF4.exe" "build\x64\%%f\SuperF4"
				copy "build\%%f\SuperF4\SuperF4.ini" "build\x64\%%f\SuperF4\SuperF4.ini"
			)
		)
	)
	
	@echo.
	echo Building installer
	if "%2" == "x64" (
		makensis /V2 /Dx64 installer.nsi
	) else (
		makensis /V2 installer.nsi
	)
) else if "%1" == "x64" (
	%prefix64%windres -o build\superf4_x64.o include\superf4.rc
	%prefix64%gcc -o SuperF4.exe superf4.c build\superf4_x64.o -mwindows -lshlwapi -lwininet -march=pentium2 -g -DDEBUG
) else (
	%prefix32%gcc -o SuperF4.exe superf4.c build\superf4.o -mwindows -lshlwapi -lwininet -march=pentium2 -g -DDEBUG
	
	if "%1" == "run" (
		start SuperF4.exe
	)
	if "%1" == "hide" (
		start SuperF4.exe -hide
	)
)
