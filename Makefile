L10N = en-US es-ES gl-ES

debug: pre
	gcc -o SuperF4_x64.exe superf4.c build/superf4_x64.o -mwindows -lshlwapi -lwininet -DDEBUG

all: pre
	@echo Building binaries
	gcc -o build/ini.exe include/ini.c -lshlwapi
	
	mkdir -p build/x64/en-US/SuperF4
	gcc -o build/x64/en-US/SuperF4/SuperF4.exe superf4.c build/superf4_x64.o -mwindows -lshlwapi -lwininet -O2
	strip build/x64/en-US/SuperF4/SuperF4.exe
	@for lang in ${L10N} ;\
	do \
		echo Putting together $$lang ;\
		mkdir -p build/x64/$$lang/SuperF4 ;\
		if [ $$lang != en-US ] ;\
		then \
			cp build/x64/en-US/SuperF4/SuperF4.exe build/x64/$$lang/SuperF4 ;\
		fi ;\
		cp localization/$$lang/info.txt build/x64/$$lang/SuperF4 ;\
		cp SuperF4.ini build/x64/$$lang/SuperF4 ;\
		build/ini.exe build/x64/$$lang/SuperF4/SuperF4.ini SuperF4 Language $$lang ;\
	done
	
pre:
	-taskkill -IM SuperF4_x64.exe
	-mkdir build
	windres -o build/superf4_x64.o include/superf4_x64.rc
