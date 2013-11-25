; �ýű�ʹ�� HM VNISEdit �ű��༭���򵼲���

; ��װ�����ʼ���峣��
!include LogicLib.nsh
!include WinVer.nsh
!include WinMessages.nsh
!include FileFunc.nsh
!include WordFunc.nsh

; my header
!include iDesktopInit.nsh
!include SharedMacro.nsh
;  ***********  ******************
Var Bool_Finishpage_Run

!include InstSuccess.nsh

; old header
!include old\EnvVarUpdate.nsh
!include old\WindowsVersion.nsh
!include old\String.nsh

!define PRODUCT_NAME "ҵ��Эͬƽ̨�ͻ���"
!define EXE_NAME "idesktop-client"
;!define INSTALLDIR "$PROGRAMFILES\${PRODUCT_NAME}"
!define PRODUCT_LONGVERSION "0.0.1.0"
!define PRODUCT_VERSION "1.0"
!define PRODUCT_PUBLISHER "i-SOFT"
!define PRODUCT_WEB_SITE "www.i-soft.com.cn"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define SETUP_NAME "ҵ��Эͬƽ̨�ͻ���"

SetCompressor lzma

;  ***********    ?    ******************
!insertmacro GetParameters
!insertmacro GetOptions

; ***************** On i-Desktop init ******************
Var  WinVersionLbl
!insertmacro iDesktopInit

; ------ MUI �ִ����涨�� (1.67 �汾���ϼ���) ------
!include "MUI.nsh"

; MUI Ԥ���峣��
!define MUI_ABORTWARNING
!define MUI_ICON "..\release\images\install.ico"
!define MUI_UNICON "..\release\images\recycleBin.ico"

; ��ӭҳ��
!insertmacro MUI_PAGE_WELCOME
; ���Э��ҳ��
!insertmacro MUI_PAGE_LICENSE "..\release\licence\LICENCE.txt"
; ��װĿ¼ѡ��ҳ��
!insertmacro MUI_PAGE_DIRECTORY
DirText "��װ���򽫰�װ $(^Name) �������ļ��С�Ҫ��װ����ͬ�ļ��У����� [���(B)] ��ѡ���������ļ��С� $_CLICK"
; ��װ����ҳ��
!insertmacro MUI_PAGE_INSTFILES
; ��װ���ҳ��

!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_FUNCTION RunApplication
!insertmacro MUI_PAGE_FINISH

Function RunApplication
	;execShell open "$DESKTOP\${SETUP_NAME}.lnk"
	IntOp $Bool_Finishpage_Run 0 + 5
FunctionEnd

; ��װж�ع���ҳ��
!insertmacro MUI_UNPAGE_INSTFILES

; ��װ�����������������
!insertmacro MUI_LANGUAGE "SimpChinese"

; ��װԤ�ͷ��ļ�
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI �ִ����涨����� ------

; *************On Install Success ****************
!insertmacro InstSuccess

; *********************** version info ******************
VIProductVersion "${PRODUCT_LONGVERSION}"
VIAddVersionKey /LANG=2052 ProductName "${SETUP_NAME}"
VIAddVersionKey /LANG=2052 ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=2052 CompanyName "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=2052 LegalTrademarks "${SETUP_NAME}"
VIAddVersionKey /LANG=2052 LegalCopyright "(C) 2013-2014"
VIAddVersionKey /LANG=2052 FileVersion "${PRODUCT_LONGVERSION}"
VIAddVersionKey /LANG=2052 FileDescription "${SETUP_NAME}"
;VIAddVersionKey /LANG=2052 OriginalFilename "${SETUP_NAME}.exe"

; *********************** Start **********************
Name "${SETUP_NAME}"
OutFile "${SETUP_NAME}.exe"
InstallDir "$PROGRAMFILES\iSOFT\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText www.i-soft.com.cn


Section "MainSection" SEC01
	SetShellVarContext all
	SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "..\release\idesktop-client.qm"
  File "..\release\${EXE_NAME}.exe"
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\${SETUP_NAME}.lnk" "$INSTDIR\${EXE_NAME}.exe"
  CreateShortCut "$DESKTOP\${SETUP_NAME}.lnk" "$INSTDIR\${EXE_NAME}.exe"
  File "..\release\QtSql4.dll"
  File "..\release\QtScript4.dll"
  File "..\release\QtNetwork4.dll"
  File "..\release\QtGui4.dll"
  File "..\release\QtCore4.dll"
  File "..\release\QtWebKit4.dll"
  File "..\release\mingwm10.dll"
  File "..\release\libstdc++-6.dll"
  File "..\release\libgcc_s_dw2-1.dll"
  File "..\release\IconGet.dll"
  File "..\release\ChildWaitDlg.dll"
  File "..\release\connet.exe"
  File "..\release\DllClientEngineMain.dll"
  File "..\release\L4CMain.dll"
  File "..\release\pluginIme.dll"
  File "..\release\PluginSeam.dll"
  File "..\release\PlugLink.dll"
  File "..\release\QtXml4.dll"
  File "..\release\changebg.dll"
  File "..\release\GetApp.dll"
  File "..\release\QJson.dll"
  !insertmacro AddVcRedist_X86
  ${If} ${IsWinXP}
  ${AndIf} ${AtLeastServicePack} 3
    File "..\files\higher\ClientEngineChildHigher.exe"
    Rename "ClientEngineChildHigher.exe" "ClientEngineChild.exe"
    File "..\files\lower\WindowsXP-KB969084-x86-chs.exe"
    nsExec::exec "$INSTDIR\WindowsXP-KB969084-x86-chs.exe /passive"
  ${EndIf}
  ${If} ${IsWinVista}
    File "..\files\higher\ClientEngineChildHigher.exe"
    Rename "ClientEngineChildHigher.exe" "ClientEngineChild.exe"
    File "..\files\higher\Windows6.0-KB969084-x86.msu"
    nsExec::exec "$INSTDIR\Windows6.0-KB969084-x86.msu"
  ${EndIf}
  ${If} ${IsWin7}
  ${OrIf} ${IsWin2008}
  ${OrIf} ${IsWin2008R2}
    File "..\files\higher\ClientEngineChildHigher.exe"
    Rename "ClientEngineChildHigher.exe" "ClientEngineChild.exe"
  ${EndIf}
  ${If} ${IsWinXP}
  ${AndIf} ${AtMostServicePack} 2
  ${OrIf} ${IsWin2003}
    File "..\files\lower\ClientEngineChildEarly.exe"
    Rename "ClientEngineChildEarly.exe" "ClientEngineChild.exe"
  ${EndIf}
  SetOutPath "$INSTDIR\imageformats"
  	File "..\release\imageformats\*.*"
  SetOutPath "$INSTDIR\sqldrivers"
		File "..\release\sqldrivers\*.*"
	SetOutPath "$INSTDIR\codecs"
		File "..\release\codecs\*.*"
	SetOutPath "$INSTDIR\images\wallpager"
  	File "..\release\images\wallpager\*.*"
  SetOutPath "$INSTDIR\images\iconWidgetBg"
  	File "..\release\images\iconWidgetBg\*.*"

  
SectionEnd

Section -AdditionalIcons
  SetShellVarContext all
  WriteIniStr "$INSTDIR\��������.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  WriteIniStr "$SMPROGRAMS\${PRODUCT_NAME}\��������.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
SectionEnd

Section -Post
	DetailPrint "ע��ע���"
	;Store installation folder
	WriteRegStr   HKLM   "Software\${PRODUCT_PUBLISHER}" '${PRODUCT_NAME}' '$INSTDIR'
  WriteRegStr   HKLM   "${PRODUCT_DIR_REGKEY}"         ""                "$INSTDIR\${EXE_NAME}.exe"
	;Create uninstaller
  WriteUninstaller     "$INSTDIR\uninst.exe"
  WriteRegStr   HKLM   "${PRODUCT_DIR_REGKEY}"         ""                "$INSTDIR\${EXE_NAME}.exe"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "DisplayName"     "$(^Name)"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "DisplayIcon"     "$INSTDIR\${EXE_NAME}.exe"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "DisplayVersion"  "${PRODUCT_LONGVERSION}"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "URLInfoAbout"    "${PRODUCT_WEB_SITE}"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "Publisher"       "${PRODUCT_PUBLISHER}"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "Comments"        "${PRODUCT_NAME}"
  WriteRegStr   HKLM   "${PRODUCT_UNINST_KEY}"         "InstallLocation" "$INSTDIR"
  WriteRegDWORD HKLM   "${PRODUCT_UNINST_KEY}"         "NoModify"        "1"
  WriteRegDWORD HKLM   "${PRODUCT_UNINST_KEY}"         "NoRepair"        "1"
SectionEnd

/******************************
 *  �����ǰ�װ�����ж�ز���  *
 ******************************/

Section Uninstall
  SetShellVarContext current
  Delete "$APPDATA\App Center\data"
	RMDir /r "$APPDATA\App Center\Licons"
	RMDir /r "$APPDATA\App Center\Picons"
	RMDir /r "$APPDATA\App Center\Vicons"
	RMDir /r "$APPDATA\App Center\tempImages"
	RMDir /r "$APPDATA\App Center"
	SetShellVarContext all
  Delete "$INSTDIR\��������.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\QJson.dll"
  Delete "$INSTDIR\IconGet.dll"
  Delete "$INSTDIR\${EXE_NAME}.qm"
  Delete "$INSTDIR\${EXE_NAME}.exe"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\mingwm10.dll"
  Delete "$INSTDIR\QtWebKit4.dll"
  Delete "$INSTDIR\QtCore4.dll"
  Delete "$INSTDIR\QtGui4.dll"
  Delete "$INSTDIR\QtNetwork4.dll"
  Delete "$INSTDIR\QtScript4.dll"
  Delete "$INSTDIR\QtSql4.dll"
  Delete "$INSTDIR\ChildWaitDlg.dll"
  Delete "$INSTDIR\ClientEngineChild.exe"
  Delete "$INSTDIR\connet.exe"
  Delete "$INSTDIR\DllClientEngineMain.dll"
  Delete "$INSTDIR\L4CMain.dll"
  Delete "$INSTDIR\pluginIme.dll"
  Delete "$INSTDIR\PluginSeam.dll"
  Delete "$INSTDIR\PlugLink.dll"
  Delete "$INSTDIR\QtXml4.dll"
  Delete "$INSTDIR\changebg.dll"
  Delete "$INSTDIR\GetApp.dll"

  RMDir /r "$INSTDIR\images\wallpager"
  RMDir /r "$INSTDIR\images\iconWidgetBg"
  RMDir /r "$INSTDIR\images"
	
  RMDir /r "$INSTDIR\codecs"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\sqldrivers"

  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\��������.lnk"
  Delete "$DESKTOP\${SETUP_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${SETUP_NAME}.lnk"

  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

#-- ���� NSIS �ű��༭�������� Function ���α�������� Section ����֮���д���Ա��ⰲװ�������δ��Ԥ֪�����⡣--#

;Function un.onInit
;/*
;	FindProcDLL::FindProc "ClientEngineChild.exe"
;	Pop $R0
;	${If} $R0 != 0
;	  KillProcDLL::KillProc "ClientEngineChild.exe"
;	${EndIf}
;
;	FindProcDLL::FindProc "${EXE_NAME}.exe"
;	Pop $R0
;	${If} $R0 != 0
;	KillProcDLL::KillProc "${EXE_NAME}.exe"
;	  MessageBox MB_ICONSTOP "$(^Name)�������У����ȹرճ���!"
;	  Quit
;	${EndIf}
;*/
;
;  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "��ȷʵҪ��ȫ�Ƴ� $(^Name) ���������е������" IDYES +2
;  Abort
  
;  nsExec::ExecToLog 'taskkill /f /im ClientEngineChild.exe'
;  nsExec::ExecToLog 'taskkill /f /im ${EXE_NAME}.exe'
;
;FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) �ѳɹ��ش����ļ�����Ƴ���"
FunctionEnd
