/*
  Copyright (C) 2020  Stefan Sundin

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

#define APP_NAME L"xkill"
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

#include "include/error.c"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
  int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  WNDCLASSEX wnd = { sizeof(WNDCLASSEX), 0, WindowProc, 0, 0, hInst, NULL, NULL, (HBRUSH)(COLOR_WINDOW+1), NULL, APP_NAME, NULL };
  wnd.hCursor = LoadImage(hInst, L"kill", IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
  RegisterClassEx(&wnd);
  HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED, wnd.lpszClassName, NULL, WS_POPUP, left, top, width, height, NULL, NULL, hInst, NULL);
  SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA); // Almost transparent
  ShowWindowAsync(hwnd, SW_SHOWNA);

  MSG msg;
  while (GetMessage(&msg,NULL,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

void Kill(HWND hwnd) {
  // Get process id
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
  if (msg == WM_DESTROY) {
    showerror = 0;
    PostQuitMessage(0);
  }
  else if (msg == WM_LBUTTONDOWN) {
    // Hide the cursor window
    ShowWindow(hwnd, SW_HIDE);

    // Get window to kill
    POINT pt;
    GetCursorPos(&pt);
    HWND target_hwnd = WindowFromPoint(pt);
    if (target_hwnd == NULL) {
      #ifdef DEBUG
      Error(L"WindowFromPoint()", L"WindowProc()", GetLastError());
      #endif
      DestroyWindow(hwnd);
      return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    target_hwnd = GetAncestor(target_hwnd, GA_ROOT);

    // Kill it!
    Kill(target_hwnd);

    // Exit
    DestroyWindow(hwnd);
  }
  else if (msg == WM_LBUTTONUP || msg == WM_MBUTTONUP || msg == WM_RBUTTONUP) {
    DestroyWindow(hwnd);
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}
