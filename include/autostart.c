/*
  Copyright (C) 2019  Stefan Sundin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
*/

// No error reporting since we don't want the user to be interrupted
int CheckAutostart() {
  // Read registry
  HKEY key;
  wchar_t value[MAX_PATH+20] = L"";
  DWORD len = sizeof(value);
  RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &key);
  RegQueryValueEx(key, APP_NAME, NULL, NULL, (LPBYTE)value, &len);
  RegCloseKey(key);
  // Compare
  wchar_t path[MAX_PATH], compare[MAX_PATH+20];
  GetModuleFileName(NULL, path, ARRAY_SIZE(path));
  swprintf(compare, ARRAY_SIZE(compare), L"\"%s\"", path);
  if (wcsstr(value,compare) != value) {
    return 0;
  }
  // Autostart is on, check arguments
  if (wcsstr(value,L" -elevate") != NULL) {
    return 2;
  }
  return 1;
}

void SetAutostart(int on, int elevate) {
  // Open key
  HKEY key;
  int error = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL);
  if (error != ERROR_SUCCESS) {
    Error(L"RegCreateKeyEx(HKEY_CURRENT_USER,'Software\\Microsoft\\Windows\\CurrentVersion\\Run')", L"Error opening the registry.", error);
    return;
  }
  if (on) {
    // Get path
    wchar_t path[MAX_PATH], value[MAX_PATH+20];
    GetModuleFileName(NULL, path, ARRAY_SIZE(path));
    swprintf(value, ARRAY_SIZE(value), L"\"%s\"%s", path, (elevate?L" -elevate":L""));
    // Set autostart
    error = RegSetValueEx(key, APP_NAME, 0, REG_SZ, (LPBYTE)value, (wcslen(value)+1)*sizeof(value[0]));
    if (error != ERROR_SUCCESS) {
      Error(L"RegSetValueEx('"APP_NAME"')", L"SetAutostart()", error);
      return;
    }
  }
  else {
    // Remove
    error = RegDeleteValue(key, APP_NAME);
    if (error != ERROR_SUCCESS) {
    Error(L"RegDeleteValue('"APP_NAME"')", L"SetAutostart()", error);
      return;
    }
  }
  // Close key
  RegCloseKey(key);
}
