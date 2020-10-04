; SuperF4 - en-US localization by Stefan Sundin
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.

!insertmacro MUI_LANGUAGE "English"
LangString L10N_LANG ${LANG_ENGLISH} ""

LangString L10N_UPGRADE_TITLE     0 "Already Installed"
LangString L10N_UPGRADE_SUBTITLE  0 "Choose how you want to install ${APP_NAME}."
LangString L10N_UPGRADE_HEADER    0 "${APP_NAME} is already installed on this system. Select the operation you want to perform and click Next to continue."
LangString L10N_UPGRADE_UPGRADE   0 "&Upgrade ${APP_NAME} to ${APP_VERSION}."
LangString L10N_UPGRADE_INI       0 "Your existing settings will be copied to ${APP_NAME}-old.ini."
LangString L10N_UPGRADE_INSTALL   0 "&Install to a new location."
LangString L10N_UPGRADE_UNINSTALL 0 "&Uninstall ${APP_NAME}."

LangString L10N_HOOKTIMEOUT_TITLE          0 "Registry tweak"
LangString L10N_HOOKTIMEOUT_SUBTITLE       0 "Optional tweak to keep ${APP_NAME} from stopping unexpectedly."
LangString L10N_HOOKTIMEOUT_HEADER         0 "Windows 7 and later has a feature that stops keyboard and mouse hooks if they take too long to respond. Unfortunately, this check can misbehave, especially if you hibernate, sleep, or lock the computer a lot.$\n$\nIf this happens, you will find that ${APP_NAME} stop functioning without warning, and you have to manually disable and enable ${APP_NAME} to make it work again.$\n$\nThere is a registry tweak to make Windows wait longer before stopping hooks, which you can enable or disable by using the buttons below. The tweak will only affect the current user. Please note that this registry tweak is completely optional."
LangString L10N_HOOKTIMEOUT_APPLYBUTTON    0 "&Enable registry tweak"
LangString L10N_HOOKTIMEOUT_REVERTBUTTON   0 "&Disable registry tweak"
LangString L10N_HOOKTIMEOUT_ALREADYAPPLIED 0 "The registry tweak has already been applied."
LangString L10N_HOOKTIMEOUT_FOOTER         0 "The change will take effect on your next login."
