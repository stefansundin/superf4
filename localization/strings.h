
struct strings {
	wchar_t *code;
	wchar_t *lang_english;
	wchar_t *lang;
	wchar_t *author;

	/* tray */
	wchar_t *tray_enabled;
	wchar_t *tray_disabled;
	struct {
		wchar_t *enable;
		wchar_t *disable;
		wchar_t *hide;
		wchar_t *update;
		wchar_t *config;
		wchar_t *about;
		wchar_t *exit;
	} menu;

	/* update */
	struct {
		wchar_t *balloon;
		wchar_t *dialog;
		wchar_t *nonew;
	} update;

	/* config */
	wchar_t *title;
	struct {
		wchar_t *general;
		wchar_t *advanced;
		wchar_t *about;
	} tabs;
	struct {
		wchar_t *box;
		wchar_t *language;
		wchar_t *autostart_box;
		wchar_t *autostart;
		wchar_t *autostart_hide;
		wchar_t *autostart_elevate;
		wchar_t *autostart_elevate_tip;
		wchar_t *elevate;
		wchar_t *elevated;
		wchar_t *elevate_tip;
		wchar_t *elevation_aborted;
		wchar_t *autosave;
	} general;
	struct {
		wchar_t *box;
		wchar_t *checkonstartup;
		wchar_t *beta;
		wchar_t *checknow;
		wchar_t *ini;
		wchar_t *openini;
	} advanced;
	struct {
		wchar_t *box;
		wchar_t *version;
		wchar_t *author;
		wchar_t *license;
		wchar_t *donate;
		wchar_t *translation_credit;
	} about;

	/* misc */
	wchar_t *unhook_error;
};

struct strings l10n_ini;

struct {
	wchar_t **str;
	wchar_t *name;
} l10n_mapping[] = {
	{ &l10n_ini.lang,                                    L"Lang" },
	{ &l10n_ini.lang_english,                            L"LangEnglish" },
	{ &l10n_ini.author,                                  L"Author" },

	{ &l10n_ini.tray_enabled,                            L"TrayEnabled" },
	{ &l10n_ini.tray_disabled,                           L"TrayDisabled" },
	{ &l10n_ini.menu.enable,                             L"MenuEnable" },
	{ &l10n_ini.menu.disable,                            L"MenuDisable" },
	{ &l10n_ini.menu.hide,                               L"MenuHideTray" },
	{ &l10n_ini.menu.update,                             L"MenuUpdateAvailable" },
	{ &l10n_ini.menu.config,                             L"MenuConfigure" },
	{ &l10n_ini.menu.about,                              L"MenuAbout" },
	{ &l10n_ini.menu.exit,                               L"MenuExit" },
	{ &l10n_ini.update.balloon,                          L"UpdateBalloon" },
	{ &l10n_ini.update.dialog,                           L"UpdateDialog" },
	{ &l10n_ini.update.nonew,                            L"UpdateLatest" },

	{ &l10n_ini.tray_disabled,                           L"MenuEnable" },
	{ &l10n_ini.menu.enable,                             L"MenuEnable" },
	{ &l10n_ini.menu.disable,                            L"MenuDisable" },
	{ &l10n_ini.menu.hide,                               L"MenuHideTray" },
	{ &l10n_ini.menu.update,                             L"MenuUpdateAvailable" },
	{ &l10n_ini.menu.config,                             L"MenuConfigure" },
	{ &l10n_ini.menu.about,                              L"MenuAbout" },
	{ &l10n_ini.menu.exit,                               L"MenuExit" },
	{ &l10n_ini.update.balloon,                          L"UpdateBalloon" },
	{ &l10n_ini.update.dialog,                           L"UpdateDialog" },
	{ &l10n_ini.update.nonew,                            L"UpdateLatest" },

	{ &l10n_ini.title,                                   L"ConfigTitle" },
	{ &l10n_ini.tabs.general,                            L"ConfigTabGeneral" },
	{ &l10n_ini.tabs.advanced,                           L"ConfigTabAdvanced" },
	{ &l10n_ini.tabs.about,                              L"ConfigTabAbout" },

	{ &l10n_ini.general.box,                             L"GeneralBox" },
	{ &l10n_ini.general.language,                        L"GeneralLanguage" },
	{ &l10n_ini.general.autostart_box,                   L"GeneralAutostartBox" },
	{ &l10n_ini.general.autostart,                       L"GeneralAutostart" },
	{ &l10n_ini.general.autostart_hide,                  L"GeneralAutostartHide" },
	{ &l10n_ini.general.autostart_elevate,               L"GeneralAutostartElevate" },
	{ &l10n_ini.general.autostart_elevate_tip,           L"GeneralAutostartElevateTip" },
	{ &l10n_ini.general.elevate,                         L"GeneralElevate" },
	{ &l10n_ini.general.elevated,                        L"GeneralElevated" },
	{ &l10n_ini.general.elevate_tip,                     L"GeneralElevateTip" },
	{ &l10n_ini.general.elevation_aborted,               L"GeneralElevationAborted" },
	{ &l10n_ini.general.autosave,                        L"GeneralAutosave" },

	{ &l10n_ini.advanced.box,                            L"AdvancedBox" },
	{ &l10n_ini.advanced.checkonstartup,                 L"AdvancedUpdateCheckOnStartup" },
	{ &l10n_ini.advanced.beta,                           L"AdvancedUpdateBeta" },
	{ &l10n_ini.advanced.checknow,                       L"AdvancedUpdateCheckNow" },
	{ &l10n_ini.advanced.ini,                            L"AdvancedIni" },
	{ &l10n_ini.advanced.openini,                        L"AdvancedOpenIni" },

	{ &l10n_ini.about.box,                               L"AboutBox" },
	{ &l10n_ini.about.version,                           L"AboutVersion" },
	{ &l10n_ini.about.author,                            L"AboutAuthor" },
	{ &l10n_ini.about.license,                           L"AboutLicense" },
	{ &l10n_ini.about.donate,                            L"AboutDonate" },
	{ &l10n_ini.about.translation_credit,                L"AboutTranslationCredit" },

	{ &l10n_ini.unhook_error,                            L"MiscUnhookError" },
};

#include "en-US/strings.h"
/*
#include "es-ES/strings.h"
#include "gl-ES/strings.h"
#include "bg-BG/strings.h"
#include "pl-PL/strings.h"
#include "it-IT/strings.h"
*/

struct strings *languages[] = {
	&en_US,
	/*
	&es_ES,
	&gl_ES,
	&bg_BG,
	&pl_PL,
	&it_IT,
	*/
};

struct strings *l10n = &en_US;
