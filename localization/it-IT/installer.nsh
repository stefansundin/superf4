;SuperF4 - it-IT localizzazione da Luciano Paravella (luciano.paravella@gmail.com)
;
;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.

!insertmacro MUI_LANGUAGE "Italian"
!define LANG ${LANG_ITALIAN}

LangString L10N_UPGRADE_TITLE     ${LANG} "Già installato"
LangString L10N_UPGRADE_SUBTITLE  ${LANG} "Scegli come si desidera installare ${APP_NAME}."
LangString L10N_UPGRADE_HEADER    ${LANG} "${APP_NAME} è già installato su questo sistema. Selezionare l'operazione che si desidera eseguire e fare clic su Avanti per continuare."
LangString L10N_UPGRADE_UPGRADE   ${LANG} "&Aggiornamento ${APP_NAME} a ${APP_VERSION}."
LangString L10N_UPGRADE_INI       ${LANG} "Le impostazioni esistenti verranno copiate ${APP_NAME}-vecchio.ini."
LangString L10N_UPGRADE_INSTALL   ${LANG} "&Installare in una nuova posizione."
LangString L10N_UPGRADE_UNINSTALL ${LANG} "&Disinstalla ${APP_NAME}."
LangString L10N_UPDATE_SECTION    ${LANG} "Verificare la presenza di aggiornamenti prima di installare"
LangString L10N_UPDATE_DIALOG     ${LANG} "Una nuova versione è disponibile.$\nInstallazione fallita e vai al sito web?"
LangString L10N_RUNNING           ${LANG} "${APP_NAME} è in esecuzione. Chiudere?"
LangString L10N_RUNNING_UNINSTALL ${LANG} "Se si sceglie No, ${APP_NAME} sarà completamente rimosso al successivo riavvio."
LangString L10N_SHORTCUT          ${LANG} "Scelta Rapida Menu di Avvio"
LangString L10N_AUTOSTART         ${LANG} "Avvio automatico"
LangString L10N_AUTOSTART_HIDE    ${LANG} "Nascondi try"

!undef LANG
