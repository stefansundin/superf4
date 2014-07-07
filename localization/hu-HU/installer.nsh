;SuperF4 - hu-HU localization by David Tamas (davestar95@gmail.com)
;
;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

!insertmacro MUI_LANGUAGE "Hungarian"
!define LANG ${LANG_HUNGARIAN}

LangString L10N_UPGRADE_TITLE     ${LANG} "Már telepítve van!"
LangString L10N_UPGRADE_SUBTITLE  ${LANG} "Válassz, hogy miként szeretnéd telepíteni a ${APP_NAME} programot."
LangString L10N_UPGRADE_HEADER    ${LANG} "${APP_NAME} már telepítve van ezen a rendszeren. Válassz egy műveletet és kattints a Következő gombra."
LangString L10N_UPGRADE_UPGRADE   ${LANG} "&Fejleszteni ${APP_NAME} ${APP_VERSION} vezióra."
LangString L10N_UPGRADE_INI       ${LANG} "A jelenlegi beállítások mentődni fognak ${APP_NAME}-old.ini fájlba."
LangString L10N_UPGRADE_INSTALL   ${LANG} "Új helyre telepítés"
LangString L10N_UPGRADE_UNINSTALL ${LANG} "&${APP_NAME} eltávolítása."
LangString L10N_UPDATE_SECTION    ${LANG} "Frissítések telepítése telepítés előtt."
LangString L10N_UPDATE_DIALOG     ${LANG} "Egy új változat érhető el.$\nMegszakítódjon a telepítés és megnyissam a weboldalt?"
LangString L10N_RUNNING           ${LANG} "${APP_NAME} jelenleg fut. Bezárjam?"
LangString L10N_RUNNING_UNINSTALL ${LANG} "Ha a Nem-et választod, akkor a ${APP_NAME} csak az újraindításkor fog törlődni."
LangString L10N_SHORTCUT          ${LANG} "Start menü parancsikon"
LangString L10N_AUTOSTART         ${LANG} "Automatkus indítás"
LangString L10N_AUTOSTART_HIDE    ${LANG} "Tálca rejtése"

!undef LANG
