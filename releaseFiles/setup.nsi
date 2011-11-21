; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Kactus2"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "TUT"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\Kactus2.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor bzip2

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Kactus2.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME}"
OutFile "Kactus2SetupWin.exe"
InstallDir "$PROGRAMFILES\Kactus2"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\executable\QtXml4.dll"
  File "..\executable\QtNetwork4.dll"
  File "..\executable\QtGui4.dll"
  File "..\executable\QtCore4.dll"
  File "..\executable\QtXmlPatterns4.dll"
  File "..\executable\Kactus2.exe"
  File "license.txt"
  File "release_notes.txt"
  File "readme.txt"
  
  SetOutPath "$INSTDIR\Kactus\internal\app_link\1.0"
  SetOverwrite try
  File "Kactus\internal\app_link\1.0\app_link.1.0.xml"
  SetOutPath "$INSTDIR\Kactus\internal\mcapi_message\1.0"
  SetOverwrite try
  File "Kactus\internal\mcapi_message\1.0\mcapi_message.1.0.xml"
  SetOutPath "$INSTDIR\Kactus\internal\mcapi_packet\1.0"
  SetOverwrite try
  File "Kactus\internal\mcapi_packet\1.0\mcapi_packet.1.0.xml"
  SetOutPath "$INSTDIR\Kactus\internal\mcapi_scalar\1.0"
  SetOverwrite try
  File "Kactus\internal\mcapi_scalar\1.0\mcapi_scalar.1.0.xml"
  
  CreateDirectory "$SMPROGRAMS\Kactus2"
  CreateShortCut "$SMPROGRAMS\Kactus2\Kactus2.lnk" "$INSTDIR\Kactus2.exe"
  CreateShortCut "$SMPROGRAMS\Kactus2\License.lnk" "$INSTDIR\license.txt"
  CreateShortCut "$SMPROGRAMS\Kactus2\Release Notes.lnk" "$INSTDIR\release_notes.txt"
  CreateShortCut "$SMPROGRAMS\Kactus2\Readme.lnk" "$INSTDIR\readme.txt"
  CreateShortCut "$DESKTOP\Kactus2.lnk" "$INSTDIR\Kactus2.exe"
SectionEnd

Section "DependencySection" SEC02
  SetOutPath "$TEMP\Kactus2Installer"
  SetOverwrite ifnewer
  File "vcredist_x86.exe"
  ExecWait '"$OUTDIR\vcredist_x86.exe" /qb'
  SetOutPath "$INSTDIR"
  RMDir /r "$TEMP\Kactus2Installer"
SectionEnd

Section -AdditionalIcons
  CreateShortCut "$SMPROGRAMS\Kactus2\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\Kactus2.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\Kactus2.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\Kactus\internal\app_link\1.0\app_link.1.0.xml"
  Delete "$INSTDIR\Kactus\internal\mcapi_message\1.0\mcapi_message.1.0.xml"
  Delete "$INSTDIR\Kactus\internal\mcapi_packet\1.0\mcapi_packet.1.0.xml"
  Delete "$INSTDIR\Kactus\internal\mcapi_scalar\1.0\mcapi_scalar.1.0.xml"
  Delete "$INSTDIR\readme.txt"
  Delete "$INSTDIR\release_notes.txt"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\Kactus2.exe"
  Delete "$INSTDIR\QtXmlPatterns4.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtXml4.dll"

  Delete "$SMPROGRAMS\Kactus2\Uninstall.lnk"
  Delete "$DESKTOP\Kactus2.lnk"
  Delete "$SMPROGRAMS\Kactus2\Readme.lnk"
  Delete "$SMPROGRAMS\Kactus2\Release Notes.lnk"
  Delete "$SMPROGRAMS\Kactus2\License.lnk"
  Delete "$SMPROGRAMS\Kactus2\Kactus2.lnk"

  RMDir "$SMPROGRAMS\Kactus2"
  RMDir "$INSTDIR\Kactus\internal\mcapi_message\1.0"
  RMDir "$INSTDIR\Kactus\internal\mcapi_message"
  RMDir "$INSTDIR\Kactus\internal\mcapi_packet\1.0"
  RMDir "$INSTDIR\Kactus\internal\mcapi_packet"
  RMDir "$INSTDIR\Kactus\internal\mcapi_scalar\1.0"
  RMDir "$INSTDIR\Kactus\internal\mcapi_scalar"
  RMDir "$INSTDIR\Kactus\internal\app_link\1.0"
  RMDir "$INSTDIR\Kactus\internal\app_link"
  RMDir "$INSTDIR\Kactus\internal"
  RMDir "$INSTDIR\Kactus"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd