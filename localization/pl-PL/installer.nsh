;SuperF4 - pl-PL localization by akul (akulewicz@o2.pl)
;
;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

!insertmacro MUI_LANGUAGE "Polish"
!define LANG ${LANG_POLISH}

LangString L10N_UPGRADE_TITLE     ${LANG} "Program jest już zainstalowany."
LangString L10N_UPGRADE_SUBTITLE  ${LANG} "Wybierz opcje instalacji ${APP_NAME}."
LangString L10N_UPGRADE_HEADER    ${LANG} "${APP_NAME} jest już zainstalowany. Wybierz co chcesz zrobić i kliknij dalej."
LangString L10N_UPGRADE_UPGRADE   ${LANG} "Aktualizuj ${APP_NAME} do wersj ${APP_VERSION}."
LangString L10N_UPGRADE_INI       ${LANG} "Twoje ustawienia zostaną skopiowane do ${APP_NAME}-old.ini."
LangString L10N_UPGRADE_INSTALL   ${LANG} "Zainstaluj w nowej lokalizacji"
LangString L10N_UPGRADE_UNINSTALL ${LANG} "Odinstaluj ${APP_NAME}."
LangString L10N_UPDATE_SECTION    ${LANG} "Sprawdź aktualizacje przed instalacją"
LangString L10N_UPDATE_DIALOG     ${LANG} "Nowa wersja jest dostępna.$\nPrzerwać i przejść na stronę?"
LangString L10N_RUNNING           ${LANG} "${APP_NAME} jest uruchomiony. Zamknąć?"
LangString L10N_RUNNING_UNINSTALL ${LANG} "Jeśli wybierzesz Nie, ${APP_NAME} zostanie całkowicie usunięty po restarcie systemu."
LangString L10N_SHORTCUT          ${LANG} "Skrót w menu start"
LangString L10N_AUTOSTART         ${LANG} "Uruchom przy starcie systemu"
LangString L10N_AUTOSTART_HIDE    ${LANG} "Ukryj ikonę zasobnika."

!undef LANG
