/*
  Copyright (C) 2015  Stefan Sundin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
*/

#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0600

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <shlwapi.h>

// App
#define APP_NAME            L"SuperF4"
#define APP_VERSION         "1.3 NoHook"
#define APP_URL             L"https://github.com/stefansundin/superf4/issues/4"

// Messages
#define WM_TRAY                   WM_USER+1
#define SWM_TOGGLE                WM_APP+1
#define SWM_ELEVATE               WM_APP+2
#define SWM_AUTOSTART_ON          WM_APP+3
#define SWM_AUTOSTART_OFF         WM_APP+4
#define SWM_AUTOSTART_ELEVATE_ON  WM_APP+5
#define SWM_AUTOSTART_ELEVATE_OFF WM_APP+6
#define SWM_WEBSITE               WM_APP+9
#define SWM_EXIT                  WM_APP+11
#define CHECKTIMER                WM_APP+12


// Boring stuff
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst = NULL;
HWND g_hwnd = NULL;
UINT WM_TASKBARCREATED = 0;
wchar_t inipath[MAX_PATH];

// Cool stuff
#define CHECKINTERVAL 50
int killing = 0; // Variable to prevent overkill
int vista = 0;
int elevated = 0;

// Include stuff
#include "localization/strings.h"
#include "include/error.c"
#include "include/autostart.c"
#include "include/tray.c"

// Entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
  g_hinst = hInst;

  // Get ini path
  GetModuleFileName(NULL, inipath, ARRAY_SIZE(inipath));
  PathRemoveFileSpec(inipath);
  wcscat(inipath, L"\\"APP_NAME".ini");
  wchar_t txt[10];

  // Convert szCmdLine to argv and argc (max 10 arguments)
  char *argv[10];
  int argc = 1;
  argv[0] = szCmdLine;
  while ((argv[argc]=strchr(argv[argc-1],' ')) != NULL) {
    *argv[argc] = '\0';
    if (argc == ARRAY_SIZE(argv)) break;
    argv[argc++]++;
  }

  // Check arguments
  int i;
  int elevate=0;
  for (i=0; i < argc; i++) {
    if (!strcmp(argv[i],"-elevate") || !strcmp(argv[i],"-e")) {
      // -elevate = create a new instance with administrator privileges
      elevate = 1;
    }
  }

  // Check if elevated if in >= Vista
  OSVERSIONINFO vi = { sizeof(OSVERSIONINFO) };
  GetVersionEx(&vi);
  vista = (vi.dwMajorVersion >= 6);
  if (vista) {
    HANDLE token;
    TOKEN_ELEVATION elevation;
    DWORD len;
    if (OpenProcessToken(GetCurrentProcess(),TOKEN_READ,&token) && GetTokenInformation(token,TokenElevation,&elevation,sizeof(elevation),&len)) {
      elevated = elevation.TokenIsElevated;
    }
  }

  // Check AlwaysElevate
  if (!elevated) {
    GetPrivateProfileString(L"Advanced", L"AlwaysElevate", L"0", txt, ARRAY_SIZE(txt), inipath);
    if (_wtoi(txt)) {
      elevate = 1;
    }

    // Handle request to elevate to administrator privileges
    if (elevate) {
      wchar_t path[MAX_PATH];
      GetModuleFileName(NULL, path, ARRAY_SIZE(path));
      int ret = (INT_PTR) ShellExecute(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
      if (ret > 32) {
        return 0;
      }
    }
  }

  // Create window
  WNDCLASSEX wnd = { sizeof(WNDCLASSEX), 0, WindowProc, 0, 0, hInst, NULL, NULL, (HBRUSH)(COLOR_WINDOW+1), NULL, APP_NAME, NULL };
  RegisterClassEx(&wnd);
  g_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED, wnd.lpszClassName, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInst, NULL);

  // Start timer
  SetTimer(g_hwnd, CHECKTIMER, CHECKINTERVAL, NULL);

  // Tray icon
  InitTray();
  UpdateTray();

  // Message loop
  MSG msg;
  while (GetMessage(&msg,NULL,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

void Kill(HWND hwnd) {
  // To prevent overkill
  if (killing) {
    return;
  }
  killing = 1;

  // Get process id of hwnd
  DWORD pid;
  GetWindowThreadProcessId(hwnd, &pid);

  int SeDebugPrivilege = 0;
  // Get process token
  HANDLE hToken;
  TOKEN_PRIVILEGES tkp;
  if (OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken) == 0) {
    // Could not elevate privileges, so we try without elevated privileges.
    #ifdef DEBUG
    Error(L"OpenProcessToken()", L"Kill()", GetLastError());
    #endif
  }
  else {
    // Get LUID for SeDebugPrivilege
    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Enable SeDebugPrivilege
    if (AdjustTokenPrivileges(hToken,FALSE,&tkp,0,NULL,0) == 0 || GetLastError() != ERROR_SUCCESS) {
      CloseHandle(hToken);
      #ifdef DEBUG
      Error(L"AdjustTokenPrivileges()", L"Kill()", GetLastError());
      #endif
    }
    else {
      // Got it
      SeDebugPrivilege = 1;
    }
  }

  // Open the process
  HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
  if (process == NULL) {
    #ifdef DEBUG
    Error(L"OpenProcess()", L"Kill()", GetLastError());
    #endif
    return;
  }

  // Terminate process
  if (TerminateProcess(process,1) == 0) {
    #ifdef DEBUG
    Error(L"TerminateProcess()", L"Kill()", GetLastError());
    #endif
    return;
  }

  // Close handle
  CloseHandle(process);

  // Disable SeDebugPrivilege
  if (SeDebugPrivilege) {
    tkp.Privileges[0].Attributes = 0;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
    CloseHandle(hToken);
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == WM_TRAY) {
    if (lParam == WM_MBUTTONDOWN) {
      if ((GetAsyncKeyState(VK_SHIFT)&0x8000)) {
        ShellExecute(NULL, L"open", inipath, NULL, NULL, SW_SHOWNORMAL);
      }
    }
    else if (lParam == WM_RBUTTONUP) {
      ShowContextMenu(hwnd);
    }
  }
  else if (msg == WM_TASKBARCREATED) {
    tray_added = 0;
    UpdateTray();
  }
  else if (msg == WM_COMMAND) {
    int wmId=LOWORD(wParam), wmEvent=HIWORD(wParam);
    if (wmId == SWM_ELEVATE) {
      wchar_t path[MAX_PATH];
      GetModuleFileName(NULL, path, ARRAY_SIZE(path));
      int ret = (INT_PTR) ShellExecute(NULL, L"runas", path, NULL, NULL, SW_SHOWNORMAL);
      if (ret > 32) {
        DestroyWindow(hwnd);
      }
    }
    else if (wmId == SWM_AUTOSTART_ON) {
      SetAutostart(1, 0);
    }
    else if (wmId == SWM_AUTOSTART_OFF) {
      SetAutostart(0, 0);
    }
    else if (wmId == SWM_AUTOSTART_ELEVATE_ON) {
      SetAutostart(1, 1);
    }
    else if (wmId == SWM_AUTOSTART_ELEVATE_OFF) {
      SetAutostart(1, 0);
    }
    else if (wmId == SWM_WEBSITE) {
      OpenUrl(APP_URL);
    }
    else if (wmId == SWM_EXIT) {
      DestroyWindow(hwnd);
    }
  }
  else if (msg == WM_DESTROY) {
    showerror = 0;
    RemoveTray();
    PostQuitMessage(0);
  }
  else if (msg == WM_TIMER) {
    if (GetAsyncKeyState(VK_LCONTROL)&0x8000
     && GetAsyncKeyState(VK_LMENU)&0x8000
     && GetAsyncKeyState(VK_F4)&0x8000) {
      // Get hwnd of foreground window
      HWND hwnd = GetForegroundWindow();
      if (hwnd == NULL) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
      }

      // Kill it!
      Kill(hwnd);
    }
    else {
      // Reset when the user has released the keys
      killing = 0;
    }
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
