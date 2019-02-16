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
    wchar_t *enable;
    wchar_t *disable;
    wchar_t *options;
    wchar_t *autostart;
    wchar_t *autostart_elevate;
    wchar_t *timercheck;
    wchar_t *open_ini;
    wchar_t *website;
    wchar_t *version;
    wchar_t *elevate;
    wchar_t *elevated;
    wchar_t *xkill;
    wchar_t *exit;
  } menu;

  wchar_t *unhook_error;
} l10n = {
  APP_NAME,
  APP_NAME " (elevated)",
  APP_NAME " (disabled)",
  {
    L"Enable",
    L"Disable",
    L"Options",
    L"Autostart",
    L"Elevate on autostart",
    L"TimerCheck",
    L"Open ini file",
    L"Open website",
    L"Version " APP_VERSION,
    L"Elevate",
    L"Elevated",
    L"xkill",
    L"Exit",
  },
  L"There was an error disabling "APP_NAME". This was most likely caused by Windows having already disabled "APP_NAME"'s hooks.",
};
