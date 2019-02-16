; Copyright (C) 2019  Stefan Sundin
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.

; For silent install you can use these switches: /S /D=C:\installdir


!define APP_NAME      "SuperF4"
!define APP_VERSION   "1.4"
!define APP_URL       "https://stefansundin.github.io/superf4/"


; Libraries

!include "MUI2.nsh"
!include "Sections.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"
!include "WinVer.nsh"
!include "x64.nsh"


; General

Name "${APP_NAME} ${APP_VERSION}"
OutFile "bin\${APP_NAME}-${APP_VERSION}.exe"
InstallDir "$APPDATA\${APP_NAME}\"
InstallDirRegKey HKCU "Software\${APP_NAME}" "Install_Dir"
RequestExecutionLevel user
ShowInstDetails hide
ShowUninstDetails show
SetCompressor /SOLID lzma

; Interface

!define MUI_LANGDLL_REGISTRY_ROOT HKCU
!define MUI_LANGDLL_REGISTRY_KEY "Software\${APP_NAME}"
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION "Launch"


; Pages

Page custom PageUpgrade PageUpgradeLeave
!define MUI_PAGE_CUSTOMFUNCTION_PRE SkipPage
!define MUI_PAGE_CUSTOMFUNCTION_SHOW HideBackButton
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
Page custom PageLowLevelHooksTimeout
!define MUI_PAGE_CUSTOMFUNCTION_SHOW MaybeDisableBackButton
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


; Variables

Var UpgradeState
Var SkipLowLevelHooksTimeoutPage

; Language

!include "localization\installer.nsh"


; Functions

!macro CloseApp un
Function ${un}CloseApp
  ; Close app if running
  FindWindow $0 "${APP_NAME}" ""
  IntCmp $0 0 done
    DetailPrint "Attempting to closing running ${APP_NAME}..."
    SendMessage $0 ${WM_CLOSE} 0 0 /TIMEOUT=500
    waitloop:
      Sleep 10
      FindWindow $0 "${APP_NAME}" ""
      IntCmp $0 0 closed waitloop waitloop
  closed:
  Sleep 100 ; Sleep a little extra to let Windows do its thing
  done:
FunctionEnd
!macroend
!insertmacro CloseApp ""
!insertmacro CloseApp "un."

; Used when upgrading to skip the directory page
Function SkipPage
  ${If} $UpgradeState == ${BST_CHECKED}
    Abort
  ${EndIf}
FunctionEnd

Function HideBackButton
  GetDlgItem $0 $HWNDPARENT 3
  ShowWindow $0 ${SW_HIDE}
FunctionEnd

Function DisableBackButton
  GetDlgItem $0 $HWNDPARENT 3
  EnableWindow $0 0
FunctionEnd

Function DisableCancelButton
  GetDlgItem $0 $HWNDPARENT 2
  EnableWindow $0 0
FunctionEnd

Function FocusNextButton
  GetDlgItem $0 $HWNDPARENT 1
  ${NSD_SetFocus} $0
FunctionEnd

Function DisableXButton
  ; Disables the close button in the title bar
  System::Call "user32::GetSystemMenu(i $HWNDPARENT, i 0) i .r1"
  System::Call "user32::EnableMenuItem(i $1, i 0xF060, i 1) v"
FunctionEnd

Function MaybeDisableBackButton
  ${If} $SkipLowLevelHooksTimeoutPage == "true"
    Call DisableBackButton
  ${EndIf}
FunctionEnd

Function Launch
  Exec "$INSTDIR\${APP_NAME}.exe"
FunctionEnd


; Installer

Section "" sec_app
  ; Close app if running
  Call CloseApp

  SetOutPath "$INSTDIR"

  ; Rename old ini file if it exists
  IfFileExists "${APP_NAME}.ini" 0 +3
    Delete "${APP_NAME}-old.ini"
    Rename "${APP_NAME}.ini" "${APP_NAME}-old.ini"

  ; Install files
  !ifdef x64
  ${If} ${RunningX64}
    File "bin\64\${APP_NAME}.exe"
  ${Else}
    File "bin\32\${APP_NAME}.exe"
  ${EndIf}
  !else
  File "bin\32\${APP_NAME}.exe"
  !endif
  File "${APP_NAME}.ini"

  ; Update registry
  WriteRegStr HKCU "Software\${APP_NAME}" "Install_Dir" "$INSTDIR"
  WriteRegStr HKCU "Software\${APP_NAME}" "Version" "${APP_VERSION}"

  ; Create uninstaller
  WriteUninstaller "Uninstall.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "QuietUninstallString" '"$INSTDIR\Uninstall.exe" /S'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayIcon" '"$INSTDIR\${APP_NAME}.exe"'
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayVersion" "${APP_VERSION}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "HelpLink" "${APP_URL}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "Publisher" "Stefan Sundin"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "InstallLocation" "$INSTDIR\"
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1

  ; Compute size for uninstall information
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "EstimatedSize" "$0"
SectionEnd

Section "" sec_shortcut
  CreateShortCut "$SMPROGRAMS\${APP_NAME}.lnk" "$INSTDIR\${APP_NAME}.exe" "" "$INSTDIR\${APP_NAME}.exe" 0
SectionEnd


; LowLevelHooksTimeout notification
Var AdjustLowLevelHooksTimeoutButton
Var RevertLowLevelHooksTimeoutButton

Function PageLowLevelHooksTimeout
  ${If} $SkipLowLevelHooksTimeoutPage == "true"
    Abort
  ${EndIf}

  nsDialogs::Create 1018
  !insertmacro MUI_HEADER_TEXT "$(L10N_HOOKTIMEOUT_TITLE)" "$(L10N_HOOKTIMEOUT_SUBTITLE)"
  ${NSD_CreateLabel} 0 0 100% 142 "$(L10N_HOOKTIMEOUT_HEADER)"

  ${NSD_CreateButton} 0 162 92u 17u "$(L10N_HOOKTIMEOUT_APPLYBUTTON)"
  Pop $AdjustLowLevelHooksTimeoutButton
  ${NSD_OnClick} $AdjustLowLevelHooksTimeoutButton AdjustLowLevelHooksTimeout

  ${NSD_CreateButton} 200 162 92u 17u "$(L10N_HOOKTIMEOUT_REVERTBUTTON)"
  Pop $RevertLowLevelHooksTimeoutButton
  ${NSD_OnClick} $RevertLowLevelHooksTimeoutButton RevertLowLevelHooksTimeout
  EnableWindow $RevertLowLevelHooksTimeoutButton 0

  ${NSD_CreateLabel} 0 195 100% 30 "$(L10N_HOOKTIMEOUT_FOOTER)"

  ; Disable buttons
  Call DisableXButton
  Call DisableCancelButton
  Call DisableBackButton

  ClearErrors
  ReadRegDWORD $0 HKCU "Control Panel\Desktop" "LowLevelHooksTimeout"
  IfErrors done

  ${NSD_CreateLabel} 0 140 100% 20 "$(L10N_HOOKTIMEOUT_ALREADYAPPLIED)"
  Pop $0
  CreateFont $1 "MS Shell Dlg" 7 700
  SendMessage $0 ${WM_SETFONT} $1 0

  EnableWindow $AdjustLowLevelHooksTimeoutButton 0
  EnableWindow $RevertLowLevelHooksTimeoutButton 1

  done:
  nsDialogs::Show
FunctionEnd

Function AdjustLowLevelHooksTimeout
  WriteRegDWORD HKCU "Control Panel\Desktop" "LowLevelHooksTimeout" 5000
  EnableWindow $AdjustLowLevelHooksTimeoutButton 0
  EnableWindow $RevertLowLevelHooksTimeoutButton 1
  Call FocusNextButton ; Otherwise Alt shortcuts won't work
FunctionEnd

Function RevertLowLevelHooksTimeout
  DeleteRegValue HKCU "Control Panel\Desktop" "LowLevelHooksTimeout"
  EnableWindow $AdjustLowLevelHooksTimeoutButton 1
  EnableWindow $RevertLowLevelHooksTimeoutButton 0
  Call FocusNextButton ; Otherwise Alt shortcuts won't work
FunctionEnd


; Detect previous installation
Var Upgradebox
Var Uninstallbox

Function PageUpgrade
  IfFileExists $INSTDIR +2
    Abort

  nsDialogs::Create 1018
  !insertmacro MUI_HEADER_TEXT "$(L10N_UPGRADE_TITLE)" "$(L10N_UPGRADE_SUBTITLE)"
  ${NSD_CreateLabel} 0 0 100% 20u "$(L10N_UPGRADE_HEADER)"

  ${NSD_CreateRadioButton} 0 45 100% 10u "$(L10N_UPGRADE_UPGRADE)"
  Pop $Upgradebox
  ${NSD_Check} $Upgradebox
  ${NSD_CreateLabel} 16 62 100% 20u "$(L10N_UPGRADE_INI)"

  ${NSD_CreateRadioButton} 0 95 100% 10u "$(L10N_UPGRADE_INSTALL)"

  ${NSD_CreateRadioButton} 0 130 100% 10u "$(L10N_UPGRADE_UNINSTALL)"
  Pop $Uninstallbox

  ${NSD_CreateLabel} 0 160 100% 30u "Note: version 1.3 and later defaults to install to the user directory. If you are upgrading from a previous version, then you are recommended to first uninstall and then install from scratch. Otherwise you have to right click the installer and use 'Run as administrator'."

  nsDialogs::Show
FunctionEnd

Function PageUpgradeLeave
  ${NSD_GetState} $Uninstallbox $0
  ${If} $0 == ${BST_CHECKED}
    ExecShell "open" '"$INSTDIR\Uninstall.exe"'
    Quit
  ${EndIf}

  ${NSD_GetState} $Upgradebox $UpgradeState
  ${If} $UpgradeState == ${BST_CHECKED}
    !insertmacro UnselectSection ${sec_shortcut}
  ${EndIf}
FunctionEnd


Function .onInit
  ; Check if special pages should appear
  StrCpy $SkipLowLevelHooksTimeoutPage "false"
  ${If} ${AtMostWinVista}
    StrCpy $SkipLowLevelHooksTimeoutPage "true"
  ${EndIf}
FunctionEnd


; Uninstaller

Section "Uninstall"
  Call un.CloseApp

  Delete /REBOOTOK "$INSTDIR\${APP_NAME}.exe"
  Delete /REBOOTOK "$INSTDIR\${APP_NAME}.ini"
  Delete /REBOOTOK "$INSTDIR\${APP_NAME}-old.ini"
  Delete /REBOOTOK "$INSTDIR\Uninstall.exe"
  RMDir  /REBOOTOK "$INSTDIR"

  Delete /REBOOTOK "$SMPROGRAMS\${APP_NAME}.lnk"

  DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "${APP_NAME}"
  DeleteRegKey /ifempty HKCU "Software\${APP_NAME}"
  DeleteRegKey /ifempty HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
SectionEnd
