/*
  Copyright (C) 2015  Stefan Sundin

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
*/

struct strings {
  wchar_t *tray_enabled;
  wchar_t *tray_elevated;
  wchar_t *tray_disabled;
  struct {
    wchar_t *options;
    wchar_t *autostart;
    wchar_t *autostart_elevate;
    wchar_t *website;
    wchar_t *version;
    wchar_t *elevate;
    wchar_t *elevated;
    wchar_t *exit;
  } menu;
} l10n = {
  APP_NAME " (NoHook)",
  APP_NAME " (NoHook elevated)",
  APP_NAME " (disabled)",
  {
    L"Options",
    L"Autostart",
    L"Elevate on autostart",
    L"Open website",
    L"Version " APP_VERSION,
    L"Elevate",
    L"Elevated",
    L"Exit",
  },
};
