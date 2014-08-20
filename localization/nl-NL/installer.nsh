;SuperF4 - nl-NL vertaald door Fred van Waard (info@office-invoegtoepassingen.nl)
;
;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

!insertmacro MUI_LANGUAGE "Dutch"
!define LANG ${LANG_DUTCH}

LangString L10N_UPGRADE_TITLE     ${LANG} "Al geïnstalleerd"
LangString L10N_UPGRADE_SUBTITLE  ${LANG} "Kies hier wat u wilt installeren ${APP_NAME}."
LangString L10N_UPGRADE_HEADER    ${LANG} "${APP_NAME} is al geïnstalleerd op dit systeem. Selecteer de bewerking die u wilt uitvoeren en klik op 'Volgende' om verder te gaan."
LangString L10N_UPGRADE_UPGRADE   ${LANG} "&Opwaarderen ${APP_NAME} naar ${APP_VERSION}."
LangString L10N_UPGRADE_INI       ${LANG} "Uw bestaande instellingen zullen gekopieerd worden naar ${APP_NAME}-old.ini."
LangString L10N_UPGRADE_INSTALL   ${LANG} "&Installeer op een nieuwe locatie."
LangString L10N_UPGRADE_UNINSTALL ${LANG} "&De-installeer ${APP_NAME}."
LangString L10N_UPDATE_SECTION    ${LANG} "Controleer voor een update voor het installeren"
LangString L10N_UPDATE_DIALOG     ${LANG} "Een nieuwe versie is beschikbaar.$\nBreek de installatie of en ga naar de website?"
LangString L10N_RUNNING           ${LANG} "${APP_NAME} draait. Sluit af?"
LangString L10N_RUNNING_UNINSTALL ${LANG} "Als u kiest voor Nee, ${APP_NAME} zal volledig verwijderd worden bij de volgende opstart."
LangString L10N_SHORTCUT          ${LANG} "Start Menu snelkoppeling"
LangString L10N_AUTOSTART         ${LANG} "Autostart"
LangString L10N_AUTOSTART_HIDE    ${LANG} "Verberg systeemvak"

!undef LANG
