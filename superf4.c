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
#include <windows.h>

//Tray messages
#define WM_ICONTRAY WM_USER+1
#define SWM_TOGGLE WM_APP+1
#define SWM_ABOUT WM_APP+2
#define SWM_EXIT WM_APP+3

//Stuff
LPSTR szClassName="SuperF4";
LRESULT CALLBACK MyWndProc(HWND, UINT, WPARAM, LPARAM);

//Hook data
static HINSTANCE hinstDLL;
static HOOKPROC hkprcSysMsg;
static HHOOK hhookSysMsg;

//Global info
static HICON icon[2];
static NOTIFYICONDATA traydata;
static UINT WM_TASKBARCREATED;
static BOOL hook_installed=FALSE;
static BOOL tray_added=FALSE;

static char msg[100];

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
	//Create window class
	WNDCLASS wnd;
	wnd.style=CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc=MyWndProc;
	wnd.cbClsExtra=0;
	wnd.cbWndExtra=0;
	wnd.hInstance=hInst;
	wnd.hIcon=LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	wnd.hCursor=LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
	wnd.hbrBackground=(HBRUSH)(COLOR_BACKGROUND+1);
	wnd.lpszMenuName=NULL;
	wnd.lpszClassName=szClassName;
	
	//Register class
	if (RegisterClass(&wnd) == 0) {
		sprintf(msg,"RegisterClass() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
		return 1;
	}
	
	//Create window
	HWND hWnd;
	hWnd=CreateWindow(szClassName, "SuperF4", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);
	//ShowWindow(hWnd, iCmdShow); //Show
	//UpdateWindow(hWnd); //Update
	
	//Register TaskbarCreated so we can readd the tray icon if explorer.exe crashes
	if ((WM_TASKBARCREATED=RegisterWindowMessage("TaskbarCreated")) == 0) {
		sprintf(msg,"RegisterWindowMessage() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
	}
	
	//Load tray icons
	if ((icon[0] = LoadImage(hInst, "tray-disabled", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR)) == NULL) {
		sprintf(msg,"LoadImage() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
		return 1;
	}
	if ((icon[1] = LoadImage(hInst, "tray-enabled", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR)) == NULL) {
		sprintf(msg,"LoadImage() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Error", MB_ICONERROR|MB_OK);
		return 1;
	}
	
	//Create icondata
	traydata.cbSize=sizeof(NOTIFYICONDATA);
	traydata.uID=0;
	traydata.uFlags=NIF_MESSAGE+NIF_ICON+NIF_TIP;
	traydata.hWnd=hWnd;
	traydata.uCallbackMessage=WM_ICONTRAY;
	strncpy(traydata.szTip,"SuperF4 (disabled)",sizeof(traydata.szTip));
	traydata.hIcon=icon[0];
	
	//Add tray icon
	AddTray();
	
	//Install hook
	InstallHook();
	
	//Message loop
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

void ShowContextMenu(HWND hWnd) {
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if (hMenu) {
		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_TOGGLE, (hook_installed?"Disable":"Enable"));
		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_ABOUT, "About");
		InsertMenu(hMenu, -1, MF_BYPOSITION, SWM_EXIT, "Exit");

		//Must set window to the foreground, or else the menu won't disappear when clicking outside it
		SetForegroundWindow(hWnd);

		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL );
		DestroyMenu(hMenu);
	}
}

int AddTray() {
	if (tray_added) {
		//Tray already added
		return 1;
	}
	
	if (Shell_NotifyIcon(NIM_ADD,&traydata) == FALSE) {
		sprintf(msg,"Shell_NotifyIcon() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Success
	tray_added=TRUE;
}

int RemoveTray() {
	if (!tray_added) {
		//Tray not added
		return 1;
	}
	
	if (Shell_NotifyIcon(NIM_DELETE,&traydata) == FALSE) {
		sprintf(msg,"Shell_NotifyIcon() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Success
	tray_added=FALSE;
}

int InstallHook() {
	if (hook_installed) {
		//Hook already installed
		return 1;
	}
	
	//Load dll
	if ((hinstDLL=LoadLibrary((LPCTSTR)"keyhook.dll")) == NULL) {
		sprintf(msg,"LoadLibrary() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Get address to keyboard hook (beware name mangling)
	if ((hkprcSysMsg=(HOOKPROC)GetProcAddress(hinstDLL,"KeyboardProc@12")) == NULL) {
		sprintf(msg,"GetProcAddress() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Set up the hook
	if ((hhookSysMsg=SetWindowsHookEx(WH_KEYBOARD_LL,hkprcSysMsg,hinstDLL,0)) == NULL) {
		sprintf(msg,"SetWindowsHookEx() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Success
	hook_installed=TRUE;
	traydata.hIcon=icon[1];
	strncpy(traydata.szTip,"SuperF4 (enabled)",sizeof(traydata.szTip));
	if (tray_added) {
		if (Shell_NotifyIcon(NIM_MODIFY,&traydata) == FALSE) {
			sprintf(msg,"Shell_NotifyIcon() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
			MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
			return 1;
		}
	}
	return 0;
}

int RemoveHook() {
	if (!hook_installed) {
		//Hook not installed
		return 1;
	}
	
	//Remove hook
	if (UnhookWindowsHookEx(hhookSysMsg) == 0) {
		sprintf(msg,"UnhookWindowsHookEx() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Unload dll
	if (FreeLibrary(hinstDLL) == 0) {
		sprintf(msg,"FreeLibrary() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
		MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
		return 1;
	}
	
	//Success
	hook_installed=FALSE;
	traydata.hIcon=icon[0];
	strncpy(traydata.szTip,"SuperF4 (disabled)",sizeof(traydata.szTip));
	if (tray_added) {
		if (Shell_NotifyIcon(NIM_MODIFY,&traydata) == FALSE) {
			sprintf(msg,"Shell_NotifyIcon() failed (error code: %d) in file %s, line %d.",GetLastError(),__FILE__,__LINE__);
			MessageBox(NULL, msg, "SuperF4 Warning", MB_ICONWARNING|MB_OK);
			return 1;
		}
	}
	return 0;
}

void ToggleHook() {
	if (hook_installed) {
		RemoveHook();
	}
	else {
		InstallHook();
	}
}

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_COMMAND) {
		int wmId=LOWORD(wParam), wmEvent=HIWORD(wParam);
		if (wmId == SWM_TOGGLE) {
			ToggleHook();
		}
		else if (wmId == SWM_ABOUT) {
			MessageBox(NULL, "SuperF4 - 0.2\nrecover89@gmail.com\nhttp://superf4.googlecode.com/\n\nWhen enabled, press Ctrl+Alt+F4 to kill the process of the currently selected window.\nThe effect is the same as when you kill the process from the task manager.\n\nSend feedback to recover89@gmail.com", "About SuperF4", MB_ICONINFORMATION|MB_OK);
		}
		else if (wmId == SWM_EXIT) {
			DestroyWindow(hWnd);
		}
	}
	else if (msg == WM_ICONTRAY) {
		if (lParam == WM_LBUTTONDOWN) {
			ToggleHook();
		}
		else if (lParam == WM_RBUTTONDOWN) {
			ShowContextMenu(hWnd);
		}
	}
	else if (msg == WM_TASKBARCREATED) {
		tray_added=FALSE;
		AddTray();
	}
	else if (msg == WM_DESTROY) {
		if (hook_installed) {
			RemoveHook();
		}
		if (tray_added) {
			RemoveTray();
		}
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
