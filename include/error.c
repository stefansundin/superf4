/*
  Copyright (C) 2020  Stefan Sundin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
*/

int showerror = 1;


LRESULT CALLBACK ErrorMsgProc(INT nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HCBT_ACTIVATE) {
    // Edit the caption of the buttons
    SetDlgItemText((HWND)wParam, IDYES, L"Copy error");
    SetDlgItemText((HWND)wParam, IDNO,  L"OK");
  }
  return 0;
}

void _Error(wchar_t *func, wchar_t *info, int errorcode, wchar_t *file, int line) {
  if (!showerror) {
    return;
  }
  // Format message
  wchar_t msg[1000], *errormsg;
  int length = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorcode, 0, (wchar_t*)&errormsg, 0, NULL);
  if (length != 0) {
    errormsg[length-2] = '\0'; // Remove that damn newline at the end of the formatted error message
  }
  swprintf(msg, ARRAY_SIZE(msg), L"%s failed in file %s, line %d.\nError: %s (%d)\n\n%s", func, file, line, errormsg, errorcode, info);
  LocalFree(errormsg);
  // Display message
  #ifdef ERROR_WRITETOFILE
  FILE *f = OpenLog(L"ab");
  fputws(msg, f);
  fputws(L"\n\n", f);
  CloseLog(f);
  #else
  // Tip: You can also press Ctrl+C in a MessageBox window to copy the text
  HHOOK hhk = SetWindowsHookEx(WH_CBT, &ErrorMsgProc, 0, GetCurrentThreadId());
  int response = MessageBox(NULL, msg, APP_NAME" Error", MB_ICONERROR|MB_YESNO|MB_DEFBUTTON2);
  UnhookWindowsHookEx(hhk);
  if (response == IDYES) {
    // Copy message to clipboard
    int size = (wcslen(msg)+1)*sizeof(msg[0]);
    wchar_t *data = LocalAlloc(LMEM_FIXED, size);
    memcpy(data, msg, size);
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, data);
    CloseClipboard();
    LocalFree(data);
  }
  #endif
}

#define Error(a,b,c) _Error(a, b, c, TEXT(__FILE__), __LINE__)
