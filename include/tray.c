/*
	Tray functions.
	Copyright (C) 2009  Stefan Sundin (recover89@gmail.com)
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

NOTIFYICONDATA tray;
int tray_added = 0;
int hide = 0;

int UpdateTray() {
	wcsncpy(tray.szTip, (enabled()?l10n->tray_enabled:l10n->tray_disabled), sizeof(tray.szTip)/sizeof(wchar_t));
	tray.hIcon = icon[enabled()?1:0];
	
	//Only add or modify if not hidden or if balloon will be displayed
	if (!hide || tray.uFlags&NIF_INFO) {
		int tries = 0; //Try at least a hundred times, sleep 100 ms between each attempt
		while (Shell_NotifyIcon((tray_added?NIM_MODIFY:NIM_ADD),&tray) == FALSE) {
			tries++;
			if (tries >= 100) {
				Error(L"Shell_NotifyIcon(NIM_ADD/NIM_MODIFY)", L"Failed to add/update tray icon.", GetLastError(), TEXT(__FILE__), __LINE__);
				return 1;
			}
			Sleep(100);
		}
		
		//Success
		tray_added = 1;
	}
	return 0;
}

int RemoveTray() {
	if (!tray_added) {
		//Tray not added
		return 1;
	}
	
	if (Shell_NotifyIcon(NIM_DELETE,&tray) == FALSE) {
		Error(L"Shell_NotifyIcon(NIM_DELETE)", L"Failed to remove tray icon.", GetLastError(), TEXT(__FILE__), __LINE__);
		return 1;
	}
	
	//Success
	tray_added = 0;
	return 0;
}
