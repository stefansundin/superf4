L10N = en-US es-ES gl-ES

debug: pre
	gcc -o SuperF4_x64.exe superf4.c build/superf4_x64.o -mwindows -lshlwapi -lwininet -DDEBUG

all: pre
	@echo Building binaries
	
	mkdir -p "build/en-US/SuperF4 (x64)"
	gcc -o "build/ini.exe" include/ini.c -lshlwapi
	gcc -o "build/en-US/SuperF4 (x64)/SuperF4_x64.exe" superf4.c build/superf4_x64.o -mwindows -lshlwapi -lwininet -O2
	strip "build/en-US/SuperF4 (x64)/SuperF4_x64.exe"
	@for lang in ${L10N} ;\
	do \
		echo Putting together $$lang ;\
		mkdir -p "build/$$lang/SuperF4 (x64)" ;\
		if [ $$lang != "en-US" ] ;\
		then \
			cp "build/en-US/SuperF4 (x64)/SuperF4_x64.exe" "build/$$lang/SuperF4 (x64)" ;\
		fi ;\
		cp localization/$$lang/info.txt "build/$$lang/SuperF4 (x64)" ;\
		cp SuperF4.ini "build/$$lang/SuperF4 (x64)" ;\
		build/ini.exe "build/$$lang/SuperF4 (x64)/SuperF4.ini" SuperF4 Language $$lang ;\
	done
	
pre:
	-taskkill -IM SuperF4_x64.exe
	-mkdir build
	windres -o build/superf4_x64.o include/superf4_x64.rc
