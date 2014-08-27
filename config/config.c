/*
	Copyright (C) 2014  Stefan Sundin (recover89@gmail.com)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#include <commctrl.h>
#include <prsht.h>
#include <windowsx.h>
#include <winnt.h>

// Boring stuff
BOOL CALLBACK PropSheetProc(HWND, UINT, LPARAM);
INT_PTR CALLBACK GeneralPageDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AdvancedPageDialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutPageDialogProc(HWND, UINT, WPARAM, LPARAM);
void LinkProc(HWND, UINT, WPARAM, LPARAM);
HWND g_cfgwnd = NULL;

// Blacklist
LRESULT CALLBACK CursorProc(HWND, UINT, WPARAM, LPARAM);

// Include stuff
#include "resource.h"
#include "../include/autostart.c"

// Entry point
void OpenConfig(int startpage) {
	if (IsWindow(g_cfgwnd)) {
		PropSheet_SetCurSel(g_cfgwnd, 0, startpage);
		SetForegroundWindow(g_cfgwnd);
		return;
	}

	// Define the pages
	struct {
		int pszTemplate;
		DLGPROC pfnDlgProc;
	} pages[] = {
		{ IDD_GENERALPAGE,   GeneralPageDialogProc },
		{ IDD_ADVANCEDPAGE,  AdvancedPageDialogProc },
		{ IDD_ABOUTPAGE,     AboutPageDialogProc },
	};

	PROPSHEETPAGE psp[ARRAY_SIZE(pages)] = {};
	int i;
	for (i=0; i < ARRAY_SIZE(pages); i++) {
		psp[i].dwSize      = sizeof(PROPSHEETPAGE);
		psp[i].hInstance   = g_hinst;
		psp[i].pszTemplate = MAKEINTRESOURCE(pages[i].pszTemplate);
		psp[i].pfnDlgProc  = pages[i].pfnDlgProc;
	}

	// Define the property sheet
	PROPSHEETHEADER psh = { sizeof(PROPSHEETHEADER) };
	psh.dwFlags         = PSH_PROPSHEETPAGE | PSH_USECALLBACK | PSH_USEHICON | PSH_NOAPPLYNOW | PSH_NOCONTEXTHELP;
	psh.hwndParent      = NULL;
	psh.hInstance       = g_hinst;
	psh.hIcon           = LoadImage(g_hinst, L"app_icon", IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
	psh.pszCaption      = APP_NAME;
	psh.nPages          = ARRAY_SIZE(pages);
	psh.ppsp            = (LPCPROPSHEETPAGE) &psp;
	psh.pfnCallback     = PropSheetProc;
	psh.nStartPage      = startpage;

	// Open the property sheet
	PropertySheet(&psh);
}

void CloseConfig() {
	PostMessage(g_cfgwnd, WM_CLOSE, 0, 0);
}

void UpdateSettings() {
	PostMessage(g_hwnd, WM_UPDATESETTINGS, 0, 0);
}

void UpdateStrings() {
	// Update window title
	PropSheet_SetTitle(g_cfgwnd, 0, l10n->title);

	// Update tab titles
	HWND tc = PropSheet_GetTabControl(g_cfgwnd);
	int numrows_prev = TabCtrl_GetRowCount(tc);
	wchar_t *titles[] = { l10n->tabs.general, l10n->tabs.advanced, l10n->tabs.about };
	int i;
	for (i=0; i < ARRAY_SIZE(titles); i++) {
		TCITEM ti;
		ti.mask = TCIF_TEXT;
		ti.pszText = titles[i];
		TabCtrl_SetItem(tc, i, &ti);
	}

	// Modify UI if number of rows have changed
	int numrows = TabCtrl_GetRowCount(tc);
	if (numrows_prev != numrows) {
		HWND page = PropSheet_GetCurrentPageHwnd(g_cfgwnd);
		if (page != NULL) {
			int diffrows = numrows-numrows_prev;
			WINDOWPLACEMENT wndpl = { sizeof(WINDOWPLACEMENT) };
			// Resize window
			GetWindowPlacement(g_cfgwnd, &wndpl);
			wndpl.rcNormalPosition.bottom += 18*diffrows;
			SetWindowPlacement(g_cfgwnd, &wndpl);
			// Resize tabcontrol
			GetWindowPlacement(tc, &wndpl);
			wndpl.rcNormalPosition.bottom += 18*diffrows;
			SetWindowPlacement(tc, &wndpl);
			// Move button
			HWND button = GetDlgItem(g_cfgwnd, IDOK);
			GetWindowPlacement(button, &wndpl);
			int height = wndpl.rcNormalPosition.bottom-wndpl.rcNormalPosition.top;
			wndpl.rcNormalPosition.top += 18*diffrows;
			wndpl.rcNormalPosition.bottom = wndpl.rcNormalPosition.top+height;
			SetWindowPlacement(button, &wndpl);
			// Re-select tab
			PropSheet_SetCurSel(g_cfgwnd, page, 0);
			// Invalidate region
			GetWindowPlacement(g_cfgwnd, &wndpl);
			InvalidateRect(g_cfgwnd, &wndpl.rcNormalPosition, TRUE);
		}
	}
}

LRESULT CALLBACK PropSheetWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	LRESULT ret = DefSubclassProc(hwnd, msg, wParam, lParam);
	if (msg == WM_NCHITTEST && (ret == HTBOTTOM || ret == HTBOTTOMLEFT || ret == HTBOTTOMRIGHT || ret == HTLEFT || ret == HTTOPLEFT || ret == HTTOPRIGHT || ret == HTRIGHT || ret == HTTOP)) {
		ret = HTBORDER;
	}
	return ret;
}

BOOL CALLBACK PropSheetProc(HWND hwnd, UINT msg, LPARAM lParam) {
	if (msg == PSCB_PRECREATE) {
		DLGTEMPLATE *pDlgTemplate = (DLGTEMPLATE*) lParam;
		pDlgTemplate->style |= WS_THICKFRAME;
	}
	else if (msg == PSCB_INITIALIZED) {
		g_cfgwnd = hwnd;
		SetWindowSubclass(hwnd, PropSheetWinProc, 0, 0);
		UpdateStrings();

		// OK button replaces Cancel button
		SendMessage(g_cfgwnd, PSM_CANCELTOCLOSE, 0, 0);
		Button_Enable(GetDlgItem(g_cfgwnd,IDCANCEL), TRUE); // Re-enable to enable escape key
		WINDOWPLACEMENT wndpl = { sizeof(WINDOWPLACEMENT) };
		GetWindowPlacement(GetDlgItem(g_cfgwnd,IDCANCEL), &wndpl);
		SetWindowPlacement(GetDlgItem(g_cfgwnd,IDOK), &wndpl);
		ShowWindow(GetDlgItem(g_cfgwnd,IDCANCEL), SW_HIDE);
	}
}

INT_PTR CALLBACK GeneralPageDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	int updatestrings = 0;
	if (msg == WM_INITDIALOG) {
		wchar_t txt[20];
		int i;
		for (i=0; i < ARRAY_SIZE(languages); i++) {
			ComboBox_AddString(GetDlgItem(hwnd,IDC_LANGUAGE), languages[i]->lang);
			if (l10n == languages[i]) {
				ComboBox_SetCurSel(GetDlgItem(hwnd,IDC_LANGUAGE), i);
			}
		}
		Button_Enable(GetDlgItem(hwnd,IDC_ELEVATE), vista && !elevated);
	}
	else if (msg == WM_COMMAND) {
		int id = LOWORD(wParam);
		int event = HIWORD(wParam);
		HWND control = GetDlgItem(hwnd, id);
		int val = Button_GetCheck(control);
		wchar_t txt[10];

		if (id == IDC_TIMERCHECK) {
			WritePrivateProfileString(L"General", L"TimerCheck", _itow(val,txt,10), inipath);
		}
		else if (id == IDC_LANGUAGE && event == CBN_SELCHANGE) {
			int i = ComboBox_GetCurSel(control);
			if (i == ARRAY_SIZE(languages)) {
				OpenUrl(L"http://code.google.com/p/superf4/wiki/Translate");
				for (i=0; l10n != languages[i]; i++) {}
				ComboBox_SetCurSel(control, i);
			}
			else {
				l10n = languages[i];
				WritePrivateProfileString(L"General", L"Language", l10n->code, inipath);
				updatestrings = 1;
				UpdateStrings();
			}
		}
		else if (id == IDC_AUTOSTART) {
			SetAutostart(val, 0, 0);
			Button_Enable(GetDlgItem(hwnd,IDC_AUTOSTART_HIDE), val);
			Button_Enable(GetDlgItem(hwnd,IDC_AUTOSTART_ELEVATE), val && vista);
			if (!val) {
				Button_SetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_HIDE), BST_UNCHECKED);
				Button_SetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_ELEVATE), BST_UNCHECKED);
			}
		}
		else if (id == IDC_AUTOSTART_HIDE) {
			int elevate = Button_GetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_ELEVATE));
			SetAutostart(1, val, elevate);
		}
		else if (id == IDC_AUTOSTART_ELEVATE) {
			int hide = Button_GetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_HIDE));
			SetAutostart(1, hide, val);
			if (val) {
				// Don't nag if UAC is disabled, only check if elevated
				DWORD uac_enabled = 1;
				if (elevated) {
					DWORD len = sizeof(uac_enabled);
					HKEY key;
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_QUERY_VALUE, &key);
					RegQueryValueEx(key, L"EnableLUA", NULL, NULL, (LPBYTE)&uac_enabled, &len);
					RegCloseKey(key);
				}
				if (uac_enabled) {
					MessageBox(NULL, l10n->general.autostart_elevate_tip, APP_NAME, MB_ICONINFORMATION|MB_OK);
				}
			}
		}
		else if (id == IDC_ELEVATE && MessageBox(NULL,l10n->general.elevate_tip,APP_NAME,MB_ICONINFORMATION|MB_OK)) {
			wchar_t path[MAX_PATH];
			GetModuleFileName(NULL, path, ARRAY_SIZE(path));
			if ((INT_PTR)ShellExecute(NULL,L"runas",path,L"-config -multi",NULL,SW_SHOWNORMAL) > 32) {
				PostMessage(g_hwnd, WM_CLOSE, 0, 0);
			}
			else {
				MessageBox(NULL, l10n->general.elevation_aborted, APP_NAME, MB_ICONINFORMATION|MB_OK);
			}
			return;
		}
		UpdateSettings();
	}
	else if (msg == WM_NOTIFY) {
		LPNMHDR pnmh = (LPNMHDR)lParam;
		if (pnmh->code == PSN_SETACTIVE) {
			updatestrings = 1;

			// Autostart
			int autostart=0, hidden=0, elevated=0;
			CheckAutostart(&autostart, &hidden, &elevated);
			Button_SetCheck(GetDlgItem(hwnd,IDC_AUTOSTART), autostart?BST_CHECKED:BST_UNCHECKED);
			Button_SetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_HIDE), hidden?BST_CHECKED:BST_UNCHECKED);
			Button_SetCheck(GetDlgItem(hwnd,IDC_AUTOSTART_ELEVATE), elevated?BST_CHECKED:BST_UNCHECKED);
			Button_Enable(GetDlgItem(hwnd,IDC_AUTOSTART_HIDE), autostart);
			Button_Enable(GetDlgItem(hwnd,IDC_AUTOSTART_ELEVATE), autostart && vista);
		}
	}
	if (updatestrings) {
		// Update text
		SetDlgItemText(hwnd, IDC_GENERAL_BOX,        l10n->general.box);
		SetDlgItemText(hwnd, IDC_LANGUAGE_HEADER,    l10n->general.language);
		SetDlgItemText(hwnd, IDC_AUTOSTART_BOX,      l10n->general.autostart_box);
		SetDlgItemText(hwnd, IDC_AUTOSTART,          l10n->general.autostart);
		SetDlgItemText(hwnd, IDC_AUTOSTART_HIDE,     l10n->general.autostart_hide);
		SetDlgItemText(hwnd, IDC_AUTOSTART_ELEVATE,  l10n->general.autostart_elevate);
		SetDlgItemText(hwnd, IDC_ELEVATE,            (elevated?l10n->general.elevated:l10n->general.elevate));
		SetDlgItemText(hwnd, IDC_AUTOSAVE,           l10n->general.autosave);

		// Language
		ComboBox_DeleteString(GetDlgItem(hwnd,IDC_LANGUAGE), ARRAY_SIZE(languages));
		if (l10n == &en_US) {
			ComboBox_AddString(GetDlgItem(hwnd,IDC_LANGUAGE), L"How can I help translate?");
		}
	}
	return FALSE;
}

INT_PTR CALLBACK AdvancedPageDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_INITDIALOG) {
		wchar_t txt[10];
		GetPrivateProfileString(L"Update", L"CheckOnStartup", L"0", txt, ARRAY_SIZE(txt), inipath);
		Button_SetCheck(GetDlgItem(hwnd,IDC_CHECKONSTARTUP), _wtoi(txt)?BST_CHECKED:BST_UNCHECKED);
		GetPrivateProfileString(L"Update", L"Beta", L"0", txt, ARRAY_SIZE(txt), inipath);
		Button_SetCheck(GetDlgItem(hwnd,IDC_BETA), _wtoi(txt)?BST_CHECKED:BST_UNCHECKED);
	}
	else if (msg == WM_COMMAND) {
		if (wParam == IDC_OPENINI) {
			ShellExecute(NULL, L"open", inipath, NULL, NULL, SW_SHOWNORMAL);
		}
		else {
			wchar_t txt[10];
			int val = Button_GetCheck(GetDlgItem(hwnd,wParam));
			if (wParam == IDC_CHECKONSTARTUP) {
				WritePrivateProfileString(L"Update", L"CheckOnStartup", _itow(val,txt,10), inipath);
			}
			else if (wParam == IDC_BETA) {
				WritePrivateProfileString(L"Update", L"Beta", _itow(val,txt,10), inipath);
			}
			else if (wParam == IDC_CHECKNOW) {
				CheckForUpdate(1);
			}
		}
	}
	else if (msg == WM_NOTIFY) {
		LPNMHDR pnmh = (LPNMHDR)lParam;
		if (pnmh->code == PSN_SETACTIVE) {
			// Update text
			SetDlgItemText(hwnd, IDC_ADVANCED_BOX,    l10n->advanced.box);
			SetDlgItemText(hwnd, IDC_CHECKONSTARTUP,  l10n->advanced.checkonstartup);
			SetDlgItemText(hwnd, IDC_BETA,            l10n->advanced.beta);
			SetDlgItemText(hwnd, IDC_CHECKNOW,        l10n->advanced.checknow);
			SetDlgItemText(hwnd, IDC_OPENINI,         l10n->advanced.openini);
		}
	}
	return FALSE;
}

INT_PTR CALLBACK AboutPageDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_COMMAND) {
		if (wParam == IDC_DONATE) {
			OpenUrl(L"https://code.google.com/p/superf4/wiki/Donate");
		}
	}
	else if (msg == WM_NOTIFY) {
		LPNMHDR pnmh = (LPNMHDR)lParam;
		if (pnmh->code == PSN_SETACTIVE) {
			// Update text
			SetDlgItemText(hwnd, IDC_ABOUT_BOX,        l10n->about.box);
			SetDlgItemText(hwnd, IDC_VERSION,          l10n->about.version);
			SetDlgItemText(hwnd, IDC_AUTHOR,           l10n->about.author);
			SetDlgItemText(hwnd, IDC_LICENSE,          l10n->about.license);
			SetDlgItemText(hwnd, IDC_DONATE,           l10n->about.donate);
			SetDlgItemText(hwnd, IDC_TRANSLATIONS_BOX, l10n->about.translation_credit);

			wchar_t txt[1024] = L"";
			int i;
			for (i=0; i < ARRAY_SIZE(languages); i++) {
				wcscat(txt, languages[i]->lang_english);
				wcscat(txt, L": ");
				wcscat(txt, languages[i]->author);
				if (i+1 != ARRAY_SIZE(languages)) {
					wcscat(txt, L"\r\n");
				}
			}
			SetDlgItemText(hwnd, IDC_TRANSLATIONS, txt);
		}
	}

	LinkProc(hwnd, msg, wParam, lParam);
	return FALSE;
}

void LinkProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NOTIFY) {
		NMLINK *link = (NMLINK*)lParam;
		if (link->hdr.code == NM_CLICK || link->hdr.code == NM_RETURN) {
			OpenUrl(link->item.szUrl);
		}
	}
}
