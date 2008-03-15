/*
	SuperF4 - Force kill programs with Ctrl+Alt+F4
	Copyright (C) 2008  Stefan Sundin (recover89@gmail.com)
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

static int ctrl=0;
static int alt=0;
static int f4=0;
static char msg[100];

_declspec(dllexport) LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		int vkey=((PKBDLLHOOKSTRUCT)lParam)->vkCode;
		
		//Check if Ctrl/Alt/F4 is being pressed or released
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if (vkey == VK_CONTROL || vkey == VK_LCONTROL || vkey == VK_RCONTROL) {
				ctrl=1;
			}
			if (vkey == VK_MENU || vkey == VK_LMENU || vkey == VK_RMENU) {
				alt=1;
			}
			if (vkey == VK_F4) {
				f4=1;
			}
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			if (vkey == VK_CONTROL || vkey == VK_LCONTROL || vkey == VK_RCONTROL) {
				ctrl=0;
			}
			if (vkey == VK_MENU || vkey == VK_LMENU || vkey == VK_RMENU) {
				alt=0;
			}
			if (vkey == VK_F4) {
				f4=0;
			}
		}
		
		//Double check that Ctrl and Alt are pressed
		//This prevents a faulty kill if keyhook haven't received the KEYUP for these keys
		if (ctrl && alt && f4) {
			if (!(GetAsyncKeyState(VK_CONTROL)&0x8000)) {
				ctrl=0;
			}
			else if (!(GetAsyncKeyState(VK_MENU)&0x8000)) {
				alt=0;
			}
			/*else if (!(GetAsyncKeyState(VK_F4)&0x8000)) {
				f4=0;
			}*/
		}
		
		//This will happen if Ctrl+Alt+F4 is being pressed
		if (ctrl && alt && f4) {
			//Open log
			FILE *output;
			if ((output=fopen("log-keyhook.txt","ab")) == NULL) {
				sprintf(msg,"fopen() failed in file %s, line %d.",__FILE__,__LINE__);
				MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
				return 1;
			}
			
			//Print timestamp
			time_t rawtime;
			struct tm *timeinfo;
			time(&rawtime);
			timeinfo=localtime(&rawtime);
			strftime(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]",timeinfo);
			fprintf(output,"\n%s\n",msg);
			
			fprintf(output,"Ctrl+Alt+F4 pressed!\n");
			
			//Get hwnd of foreground window
			HWND hwnd;
			if ((hwnd=GetForegroundWindow()) == NULL) {
				sprintf(msg,"GetForegroundWindow() failed in file %s, line %d.",__FILE__,__LINE__);
				fprintf(output,"%s\n",msg);
				fclose(output);
				MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
				return 1;
			}
			
			//Get hwnd title (for log)
			char title[100];
			GetWindowText(hwnd,title,100);
			fprintf(output,"Window title: %s\n",title);
			
			//Get process id of hwnd
			DWORD pid;
			GetWindowThreadProcessId(hwnd,&pid);
			fprintf(output,"Process id: %d\n",pid);
			fprintf(output,"Killing process... ");
			
			HANDLE process;
			if ((process=OpenProcess(/*SYNCHRONIZE|*/PROCESS_TERMINATE,FALSE,pid)) == NULL) {
				fprintf(output,"failed!\n");
				sprintf(msg,"OpenProcess() failed (error: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
				fprintf(output,"%s\n",msg);
				fclose(output);
				MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
				return 1;
			}
			TerminateProcess(process,1);
			
			fprintf(output,"done!\n");
			
			//Close log
			if (fclose(output) == EOF) {
				sprintf(msg,"fclose() failed in file %s, line %d.",__FILE__,__LINE__);
				fprintf(output,"%s\n",msg);
				fclose(output);
				MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
				return 1;
			}
			
			//Prevent this keypress from being propagated to the window selected after the kill
			return 0;
		}
	}
	
    return CallNextHookEx(NULL, nCode, wParam, lParam); 
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
	return TRUE;
}
