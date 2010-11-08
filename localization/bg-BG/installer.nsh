;SuperF4 - bg-BG Преведена от Малин Желев (malinmarkov@gmail.com)
;
;Тази програма е безплатна: Вие може да я разпространявате и/или променяте.
;спрямо условията на GNU General Public License публикувана от
;the Free Software Foundation, версия 3 от лицензът или по ваш избор коя 
;да е следваща версия

!insertmacro MUI_LANGUAGE "Bulgarian"
!define LANG ${LANG_BULGARIAN}

LangString L10N_UPGRADE_TITLE     ${LANG} "Вече инсталирана"
LangString L10N_UPGRADE_SUBTITLE  ${LANG} "Изберете как искате да инсталирате  ${APP_NAME}."
LangString L10N_UPGRADE_HEADER    ${LANG} "${APP_NAME} вече е инсталирана на тази система.Изберете операцията която искате да извършите и натиснете Next"
LangString L10N_UPGRADE_UPGRADE   ${LANG} "&Обнови ${APP_NAME} до ${APP_VERSION}."
LangString L10N_UPGRADE_INI       ${LANG} "Вашите настройки ще бъдат копирани в ${APP_NAME}-old.ini"
LangString L10N_UPGRADE_INSTALL   ${LANG} "&Инсталирай на ново място."
LangString L10N_UPGRADE_UNINSTALL ${LANG} "&Премахни ${APP_NAME}."
LangString L10N_UPDATE_SECTION    ${LANG} "Провери за нова версия преди инсталацията"
LangString L10N_UPDATE_DIALOG     ${LANG} "Има нова версия.$\nСпри инсталацията и отиди на уебсаита?"
LangString L10N_RUNNING           ${LANG} "${APP_NAME} е вече стартирана. Затвори?"
LangString L10N_RUNNING_UNINSTALL ${LANG} "Ако изберете не, ${APP_NAME} ще бъде напълно премахната след следващият рестарт."
LangString L10N_SHORTCUT          ${LANG} "Старт меню shortcut"
LangString L10N_AUTOSTART         ${LANG} "Автоматичен старт"
LangString L10N_AUTOSTART_HIDE    ${LANG} "Скрита иконка"

!undef LANG
