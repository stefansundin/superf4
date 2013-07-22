
struct strings {
	wchar_t *menu_enable;
	wchar_t *menu_disable;
	wchar_t *menu_hide;
	wchar_t *menu_options;
	wchar_t *menu_autostart;
	wchar_t *menu_autohide;
	wchar_t *menu_settings;
	wchar_t *menu_chkupdate;
	wchar_t *menu_update;
	wchar_t *menu_about;
	wchar_t *menu_exit;
	wchar_t *tray_enabled;
	wchar_t *tray_disabled;
	wchar_t *update_balloon;
	wchar_t *update_dialog;
	wchar_t *update_nonew;
	wchar_t *about_title;
	wchar_t *about;
};

#include "en-US/strings.h"
#include "es-ES/strings.h"
#include "gl-ES/strings.h"
#include "bg-BG/strings.h"
#include "pl-PL/strings.h"
#include "it-IT/strings.h"

struct {
	wchar_t *code;
	struct strings *strings;
} languages[] = {
	{L"en-US", &en_US},
	{L"es-ES", &es_ES},
	{L"gl-ES", &gl_ES},
	{L"bg-BG", &bg_BG},
	{L"pl-PL", &pl_PL},
	{L"it-IT", &it_IT},
	{NULL},
};

struct strings *l10n = &en_US;
