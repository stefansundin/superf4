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
#define APP_VERSION         "1.3"
#define APP_URL             L"https://stefansundin.github.io/superf4/"

// Messages
#define WM_TRAY                   WM_USER+1
#define SWM_TOGGLE                WM_APP+1
#define SWM_ELEVATE               WM_APP+2
#define SWM_AUTOSTART_ON          WM_APP+3
#define SWM_AUTOSTART_OFF         WM_APP+4
#define SWM_AUTOSTART_ELEVATE_ON  WM_APP+5
#define SWM_AUTOSTART_ELEVATE_OFF WM_APP+6
#define SWM_TIMERCHECK_ON         WM_APP+7
#define SWM_TIMERCHECK_OFF        WM_APP+8
#define SWM_WEBSITE               WM_APP+9
#define SWM_XKILL                 WM_APP+10
#define SWM_EXIT                  WM_APP+11
#define CHECKTIMER                WM_APP+12


// Boring stuff
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ENABLED() (keyhook)
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst = NULL;
HWND g_hwnd = NULL;
UINT WM_TASKBARCREATED = 0;
UINT WM_UPDATESETTINGS = 0;
wchar_t inipath[MAX_PATH];

// Cool stuff
HHOOK keyhook = NULL;
HHOOK mousehook = NULL;
int ctrl = 0;
int alt = 0;
int win = 0;
int superkill = 0;
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

  // Register some messages
  WM_UPDATESETTINGS = RegisterWindowMessage(L"UpdateSettings");

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
  wnd.hCursor = LoadImage(hInst, L"kill", IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR);
  RegisterClassEx(&wnd);
  g_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_LAYERED, wnd.lpszClassName, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInst, NULL);
  SetLayeredWindowAttributes(g_hwnd, 0, 1, LWA_ALPHA); // Almost transparent

  // Tray icon
  InitTray();
  UpdateTray();

  // Hook keyboard
  HookKeyboard();

  // TimerCheck
  GetPrivateProfileString(L"General", L"TimerCheck", L"0", txt, ARRAY_SIZE(txt), inipath);
  if (_wtoi(txt)) {
    SetTimer(g_hwnd, CHECKTIMER, CHECKINTERVAL, NULL);
  }

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

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    int vkey = ((PKBDLLHOOKSTRUCT)lParam)->vkCode;
    if (vkey == 0x5A) {
      vkey = VK_F4;
    }

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
      // Check for Ctrl+Alt+F4
      if (vkey == VK_LCONTROL) {
        ctrl = 1;
      }
      else if (vkey == VK_LMENU) {
        alt = 1;
      }
      else if (ctrl && alt && vkey == VK_F4) {
        // Double check that Ctrl and Alt are being pressed.
        // This prevents a faulty kill if we didn't received the keyup for these keys.
        if (!(GetAsyncKeyState(VK_LCONTROL)&0x8000)) {
          ctrl = 0;
          return CallNextHookEx(NULL, nCode, wParam, lParam);
        }
        else if (!(GetAsyncKeyState(VK_LMENU)&0x8000)) {
          alt = 0;
          return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // Get hwnd of foreground window
        HWND hwnd = GetForegroundWindow();
        if (hwnd == NULL) {
          return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // Kill it!
        Kill(hwnd);

        // Prevent this keypress from being propagated
        return 1;
      }
      // Check for [the windows key]+F4
      else if (vkey == VK_LWIN) {
        win = 1;
      }
      else if (win && vkey == VK_F4) {
        // Double check that the windows button is being pressed
        if (!(GetAsyncKeyState(VK_LWIN)&0x8000)) {
          win = 0;
          return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        // Hook mouse
        HookMouse();
        // Prevent this keypress from being propagated
        return 1;
      }
      else if (vkey == VK_ESCAPE && mousehook) {
        // Unhook mouse
        UnhookMouse();
        // Prevent this keypress from being propagated
        return 1;
      }
    }
    else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
      killing = 0;
      if (vkey == VK_LCONTROL) {
        ctrl = 0;
      }
      else if (vkey == VK_LMENU) {
        alt = 0;
      }
      else if (vkey == VK_LWIN) {
        win = 0;
      }
    }
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    if (wParam == WM_LBUTTONDOWN && superkill) {
      POINT pt = ((PMSLLHOOKSTRUCT)lParam)->pt;

      // Make sure the cursor window isn't in the way
      ShowWindow(g_hwnd, SW_HIDE);

      // Get hwnd
      HWND hwnd = WindowFromPoint(pt);
      if (hwnd == NULL) {
        #ifdef DEBUG
        Error(L"WindowFromPoint()", L"LowLevelMouseProc()", GetLastError());
        #endif
        return CallNextHookEx(NULL, nCode, wParam, lParam);
      }
      hwnd = GetAncestor(hwnd, GA_ROOT);

      // Kill it!
      Kill(hwnd);

      // Unhook mouse
      UnhookMouse();

      // Prevent mousedown from propagating
      return 1;
    }
    else if (wParam == WM_RBUTTONDOWN) {
      // Disable mouse
      DisableMouse();
      // Prevent mousedown from propagating
      return 1;
    }
    else if (wParam == WM_RBUTTONUP) {
      // Unhook mouse
      UnhookMouse();
      // Prevent mouseup from propagating
      return 1;
    }
  }

  return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int HookMouse() {
  if (mousehook) {
    // Mouse already hooked
    return 1;
  }

  // Set up the mouse hook
  mousehook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hinst, 0);
  if (mousehook == NULL) {
    #ifdef DEBUG
    Error(L"SetWindowsHookEx(WH_MOUSE_LL)", L"HookMouse()", GetLastError());
    #endif
    return 1;
  }

  // Show cursor
  int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  MoveWindow(g_hwnd, left, top, width, height, FALSE);
  ShowWindowAsync(g_hwnd, SW_SHOWNA);

  // Success
  superkill = 1;
  return 0;
}

DWORD WINAPI DelayedUnhookMouse() {
  // Sleep so mouse events have time to be canceled
  Sleep(100);

  // Unhook the mouse hook
  if (UnhookWindowsHookEx(mousehook) == 0) {
    #ifdef DEBUG
    Error(L"UnhookWindowsHookEx(mousehook)", L"UnhookMouse()", GetLastError());
    #endif
    return 1;
  }

  // Success
  mousehook = NULL;
  return 0;
}

int UnhookMouse() {
  if (!mousehook) {
    // Mouse not hooked
    return 1;
  }

  // Disable
  DisableMouse();

  // Unhook
  HANDLE thread = CreateThread(NULL, 0, DelayedUnhookMouse, NULL, 0, NULL);
  CloseHandle(thread);

  // Success
  return 0;
}

int DisableMouse() {
  // Disable
  superkill = 0;
  killing = 0;

  // Hide cursor
  ShowWindow(g_hwnd, SW_HIDE);
  return 0;
}

int HookKeyboard() {
  if (keyhook) {
    // Keyboard already hooked
    return 1;
  }

  // Set up the hook
  keyhook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, g_hinst, 0);
  if (keyhook == NULL) {
    Error(L"SetWindowsHookEx(WH_KEYBOARD_LL)", L"Could not hook keyboard. Another program might be interfering.", GetLastError());
    return 1;
  }

  // Success
  UpdateTray();
  return 0;
}

int UnhookKeyboard() {
  if (!keyhook) {
    // Keyboard not hooked
    return 1;
  }

  // Remove keyboard hook
  if (UnhookWindowsHookEx(keyhook) == 0) {
    #ifdef DEBUG
    Error(L"UnhookWindowsHookEx(keyhook)", L"Could not unhook keyboard. Try restarting "APP_NAME".", GetLastError());
    #else
    if (showerror) {
      MessageBox(NULL, l10n.unhook_error, APP_NAME, MB_ICONINFORMATION|MB_OK|MB_TOPMOST|MB_SETFOREGROUND);
    }
    #endif
  }

  // Remove mouse hook (it probably isn't hooked, but just in case)
  UnhookMouse();

  // Success
  keyhook = NULL;
  UpdateTray();
  return 0;
}

void ToggleState() {
  if (ENABLED()) {
    UnhookKeyboard();
    KillTimer(g_hwnd, CHECKTIMER);
  }
  else {
    SendMessage(g_hwnd, WM_UPDATESETTINGS, 0, 0);
    HookKeyboard();
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (msg == WM_TRAY) {
    if (lParam == WM_LBUTTONDOWN || lParam == WM_LBUTTONDBLCLK) {
      ToggleState();
    }
    else if (lParam == WM_MBUTTONDOWN) {
      if ((GetAsyncKeyState(VK_SHIFT)&0x8000)) {
        ShellExecute(NULL, L"open", inipath, NULL, NULL, SW_SHOWNORMAL);
      }
      else {
        HookMouse();
      }
    }
    else if (lParam == WM_RBUTTONUP) {
      ShowContextMenu(hwnd);
    }
  }
  else if (msg == WM_UPDATESETTINGS) {
    wchar_t txt[10];
    // TimerCheck
    KillTimer(g_hwnd, CHECKTIMER);
    GetPrivateProfileString(L"General", L"TimerCheck", L"0", txt, ARRAY_SIZE(txt), inipath);
    if (_wtoi(txt)) {
      SetTimer(g_hwnd, CHECKTIMER, CHECKINTERVAL, NULL);
    }
  }
  else if (msg == WM_TASKBARCREATED) {
    tray_added = 0;
    UpdateTray();
  }
  else if (msg == WM_COMMAND) {
    int wmId=LOWORD(wParam), wmEvent=HIWORD(wParam);
    if (wmId == SWM_TOGGLE) {
      ToggleState();
    }
    else if (wmId == SWM_ELEVATE) {
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
    else if (wmId == SWM_TIMERCHECK_ON) {
      WritePrivateProfileString(L"General", L"TimerCheck", L"1", inipath);
      SendMessage(g_hwnd, WM_UPDATESETTINGS, 0, 0);
    }
    else if (wmId == SWM_TIMERCHECK_OFF) {
      WritePrivateProfileString(L"General", L"TimerCheck", L"0", inipath);
      SendMessage(g_hwnd, WM_UPDATESETTINGS, 0, 0);
    }
    else if (wmId == SWM_WEBSITE) {
      OpenUrl(APP_URL);
    }
    else if (wmId == SWM_XKILL) {
      HookMouse();
    }
    else if (wmId == SWM_EXIT) {
      DestroyWindow(hwnd);
    }
  }
  else if (msg == WM_DESTROY) {
    showerror = 0;
    UnhookKeyboard();
    UnhookMouse();
    RemoveTray();
    PostQuitMessage(0);
  }
  else if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
    // Hide the window if clicked on, this might happen if it wasn't hidden by the hooks for some reason
    ShowWindow(hwnd, SW_HIDE);
    // Since we take away the skull, make sure we can't kill anything
    UnhookMouse();
  }
  else if (msg == WM_TIMER && ENABLED()) {
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
