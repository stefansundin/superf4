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
static int win=0;
static char msg[100];
static FILE *output;

static HINSTANCE hinstDLL;
static HHOOK mousehook;
static int hook_installed=0;

BOOL SetPrivilege(HANDLE hToken, LPCTSTR priv, BOOL bEnablePrivilege) {
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious=sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, priv, &luid)) {
		return FALSE;
	}
	
	//Get current privileges
	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid=luid;
	tp.Privileges[0].Attributes=0;

	if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &tpPrevious, &cbPrevious) != 0 && GetLastError() != ERROR_SUCCESS) {
		return FALSE;
	}

	//Set privileges
	tpPrevious.PrivilegeCount=1;
	tpPrevious.Privileges[0].Luid=luid;

	if(bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED & tpPrevious.Privileges[0].Attributes);
	}

	if (AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL) != 0 && GetLastError() != ERROR_SUCCESS) {
		return FALSE;
	}

	return TRUE;
}

char* GetTimestamp(char *buf, size_t maxsize, char *format) {
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(buf,maxsize,format,timeinfo);
	return buf;
}

void Kill(HWND hwnd) {
	fprintf(output,"%s ",GetTimestamp(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]"));
	
	//Get hwnd title (for log)
	char title[100];
	GetWindowText(hwnd,title,100);
	
	//Get process id of hwnd
	DWORD pid;
	GetWindowThreadProcessId(hwnd,&pid);
	
	fprintf(output,"Killing \"%s\" (pid %d)... ",title,pid);
	
	//Open the process
	HANDLE process;
	if ((process=OpenProcess(PROCESS_TERMINATE,FALSE,pid)) == NULL) {
		fprintf(output,"failed!\n");
		fprintf(output,"Error: OpenProcess() failed (error: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
		fflush(output);
		return;
	}
	
	//Terminate process
	if (TerminateProcess(process,1) == 0) {
		fprintf(output,"failed!\n");
		fprintf(output,"Error: TerminateProcess() failed (error: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
		fflush(output);
		return;
	}
	
	fprintf(output,"success!\n");
	fflush(output);
}

_declspec(dllexport) LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		int vkey=((PKBDLLHOOKSTRUCT)lParam)->vkCode;
		
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			//Check for Ctrl+Alt+F4
			if (vkey == VK_LCONTROL) {
				ctrl=1;
			}
			if (vkey == VK_LMENU) {
				alt=1;
			}
			if (ctrl && alt && vkey == VK_F4) {
				//Double check that Ctrl and Alt are being pressed
				//This prevents a faulty kill if we didn't received the keyup for these keys
				if (!(GetAsyncKeyState(VK_LCONTROL)&0x8000)) {
					ctrl=0;
				}
				else if (!(GetAsyncKeyState(VK_LMENU)&0x8000)) {
					alt=0;
				}
				else {
					//Get hwnd of foreground window
					HWND hwnd;
					if ((hwnd=GetForegroundWindow()) == NULL) {
						fprintf(output,"Error: GetForegroundWindow() failed in file %s, line %d.",__FILE__,__LINE__);
						fflush(output);
						return 0;
					}
					
					//Kill it!
					Kill(hwnd);
					
					//Prevent this keypress from being propagated
					return 1;
				}
			}
			//Check for [the windows key]+F4
			if (vkey == VK_LWIN) {
				win=1;
			}
			if (win && vkey == VK_F4) {
				//Double check that the windows button is being pressed
				if (!(GetAsyncKeyState(VK_LWIN)&0x8000)) {
					win=0;
				}
				else {
					//Install hook
					InstallHook();
					//Prevent this keypress from being propagated
					return 1;
				}
			}
			if (vkey == VK_ESCAPE && hook_installed) {
				//Remove hook
				RemoveHook();
				//Prevent this keypress from being propagated
				return 1;
			}
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			if (vkey == VK_LCONTROL) {
				ctrl=0;
			}
			if (vkey == VK_LMENU) {
				alt=0;
			}
			if (vkey == VK_LMENU) {
				win=0;
			}
		}
	}
	
    return CallNextHookEx(NULL, nCode, wParam, lParam); 
}

_declspec(dllexport) LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode == HC_ACTION) {
		if (wParam == WM_LBUTTONDOWN) {
			POINT pt=((PMSLLHOOKSTRUCT)lParam)->pt;
			
			//Get hwnd
			HWND hwnd;
			if ((hwnd=WindowFromPoint(pt)) == NULL) {
				fprintf(output,"%s Error getting mouse coordinates.\n",GetTimestamp(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]"));
				fprintf(output,"WindowFromPoint() failed in file %s, line %d.\n",__FILE__,__LINE__);
			}
			hwnd=GetAncestor(hwnd,GA_ROOT);
			
			//Kill it!
			Kill(hwnd);
			
			//Remove hook
			RemoveHook();
			
			//Prevent mousedown from propagating
			return 1;
		}
		else if (wParam == WM_RBUTTONDOWN) {
			//Remove hook
			RemoveHook();
			//Prevent mousedown from propagating
			return 1;
		}
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int InstallHook() {
	if (hook_installed) {
		//Hook already installed
		return 1;
	}
	
	//Set up the mouse hook
	if ((mousehook=SetWindowsHookEx(WH_MOUSE_LL,MouseProc,hinstDLL,0)) == NULL) {
		fprintf(output,"%s Error hooking mouse.\n",GetTimestamp(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]"));
		fprintf(output,"SetWindowsHookEx() failed (error code: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
		return 1;
	}
	
	//Success
	hook_installed=1;
	return 0;
}

int RemoveHook() {
	if (!hook_installed) {
		//Hook not installed
		return 1;
	}
	
	//Remove mouse hook
	if (UnhookWindowsHookEx(mousehook) == 0) {
		fprintf(output,"%s Error unhooking mouse.\n",GetTimestamp(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]"));
		fprintf(output,"UnhookWindowsHookEx() failed (error code: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
		return 1;
	}
	
	//Success
	hook_installed=0;
	return 0;
}

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		hinstDLL=hInstance;
		
		//Open log
		output=fopen("superf4-log.txt","ab");
		fprintf(output,"\n%s ",GetTimestamp(msg,sizeof(msg),"[%Y-%m-%d %H:%M:%S]"));
		fprintf(output,"New session. Getting SeDebugPrivilege privilege... ");
		
		//Create security context
		if (ImpersonateSelf(SecurityImpersonation) == 0) {
			fprintf(output,"failed!\n");
			fprintf(output,"Error: ImpersonateSelf() failed (error: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
			fflush(output);
			return TRUE;
		}
		//Get access token
		HANDLE hToken;
		if (OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken) == 0) {
			fprintf(output,"failed!\n");
			fprintf(output,"Error: OpenThreadToken() failed (error: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
			fflush(output);
			return TRUE;
		}
		//Enable SeDebugPrivilege
		if (SetPrivilege(hToken, SE_DEBUG_NAME, TRUE) == FALSE) {
			fprintf(output,"failed!\n");
			fprintf(output,"Error: SetPrivilege() failed (error: %d) in file %s, line %d.\n",GetLastError(),__FILE__,__LINE__);
			fflush(output);
			CloseHandle(hToken);
			return TRUE;
		}
		CloseHandle(hToken);
		fprintf(output,"success!\n");
		fflush(output);
	}
	else if (reason == DLL_PROCESS_ATTACH) {
		//Close log
		fclose(output);
	}

	return TRUE;
}
