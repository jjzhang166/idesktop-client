; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

; 安装程序初始定义常量
!include LogicLib.nsh
!include WinVer.nsh
!include WinMessages.nsh
; old header
!include old\EnvVarUpdate.nsh
!include old\WindowsVersion.nsh
!include old\String.nsh

!define PRODUCT_NAME "iDesktop-client"
!define EXE_NAME "idesktop-client"
;!define INSTALLDIR "$PROGRAMFILES\${PRODUCT_NAME}"
!define PRODUCT_LONGVERSION "0.0.0.1"
!define PRODUCT_VERSION "0.1"
!define PRODUCT_PUBLISHER "i-SOFT"
!define PRODUCT_WEB_SITE "www.i-soft.com.cn"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\${PRODUCT_NAME}.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define SETUP_NAME "普华统一桌面平台"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"

; MUI 预定义常量
!define MUI_ABORTWARNING
!define MUI_ICON "..\release\images\install.ico"
!define MUI_UNICON "..\release\images\recycleBin.ico"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME
; 许可协议页面
!insertmacro MUI_PAGE_LICENSE "..\release\licence\LICENCE.txt"
; 安装目录选择页面
!insertmacro MUI_PAGE_DIRECTORY
DirText "安装程序将安装 $(^Name) 在下列文件夹。要安装到不同文件夹，单击 [浏览(B)] 并选择其他的文件夹。 $_CLICK"
; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
; 安装完成页面
!define MUI_FINISHPAGE_RUN "$INSTDIR\${EXE_NAME}.exe"
!insertmacro MUI_PAGE_FINISH

; 安装卸载过程页面
!insertmacro MUI_UNPAGE_INSTFILES

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

; 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS
; ------ MUI 现代界面定义结束 ------

; *********************** version info ******************
VIProductVersion "${PRODUCT_LONGVERSION}"
VIAddVersionKey /LANG=2052 ProductName "${SETUP_NAME}"
VIAddVersionKey /LANG=2052 ProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=2052 CompanyName "${PRODUCT_PUBLISHER}"
VIAddVersionKey /LANG=2052 LegalTrademarks "${SETUP_NAME}"
VIAddVersionKey /LANG=2052 LegalCopyright "(C) 2013-2014"
VIAddVersionKey /LANG=2052 FileVersion "${PRODUCT_LONGVERSION}"
VIAddVersionKey /LANG=2052 FileDescription "${SETUP_NAME}"

; *********************** Start **********************
Name "${SETUP_NAME}"
OutFile "${SETUP_NAME}.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"
ShowInstDetails show
ShowUnInstDetails show
BrandingText www.i-soft.com.cn

Section "MainSection" SEC01
	SetShellVarContext all
	SetOutPath "$INSTDIR"
	SetOutPath "$INSTDIR\imageformats"
  File "..\release\imageformats\*.*"
  SetOutPath "$INSTDIR\sqldrivers"
	File "..\release\sqldrivers\*.*"
	SetOutPath "$INSTDIR\codecs"
	File "..\release\codecs\*.*"
	#SetOutPath "$INSTDIR\system_manage"
	#File "..\release\system_manage\*.*"
	#SetOutPath "$INSTDIR\system_manage\js"
	#File "..\release\system_manage\js\*.*"
	#SetOutPath "$INSTDIR\system_manage\css"
	#File "..\release\system_manage\css\*.*"
	#SetOutPath "$INSTDIR\system_manage\images"
	#File "..\release\system_manage\images\*.*"
	SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
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
  #File "..\release\ClientEngineChild.exe"
  File "..\release\connet.exe"
  File "..\release\DllClientEngineMain.dll"
  File "..\release\L4CMain.dll"
  File "..\release\pluginIme.dll"
  File "..\release\PluginSeam.dll"
  File "..\release\PlugLink.dll"
  File "..\release\QtXml4.dll"
  File "..\release\changebg.dll"
  File "..\release\GetApp.dll"
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
	SetOutPath "$INSTDIR\images\wallpager"
  File "..\release\images\wallpager\*.*"
  
SectionEnd

Section -AdditionalIcons
  SetShellVarContext all
  WriteIniStr "$INSTDIR\${SETUP_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninst.exe"
  WriteIniStr "$SMPROGRAMS\${PRODUCT_NAME}\关于我们.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
SectionEnd

Section -Post
	DetailPrint "注册注册表"
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
 *  以下是安装程序的卸载部分  *
 ******************************/

Section Uninstall
	SetShellVarContext all
  Delete "$INSTDIR\${SETUP_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\IconGet.dll"
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

  RMDir /r "$INSTDIR\wallpager\*.*"
  RMDir /r "$INSTDIR\wallpager"
	
  RMDir /r "$INSTDIR\codecs"
  RMDir /r "$INSTDIR\imageformats"
  RMDir /r "$INSTDIR\sqldrivers"
  #RMDir /r "$INSTDIR\system_manage\js"
  #RMDir /r "$INSTDIR\system_manage\css"
  #RMDir /r "$INSTDIR\system_manage\images"
  #RMDir /r "$INSTDIR\system_manage"

  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\关于我们.lnk"
  Delete "$DESKTOP\${SETUP_NAME}.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\${SETUP_NAME}.lnk"

  RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"

  RMDir /r "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

Function un.onInit
/*
	FindProcDLL::FindProc "ClientEngineChild.exe"
	Pop $R0
	${If} $R0 != 0
	  KillProcDLL::KillProc "ClientEngineChild.exe"
	${EndIf}
	
	FindProcDLL::FindProc "${EXE_NAME}.exe"
	Pop $R0
	${If} $R0 != 0
	KillProcDLL::KillProc "${EXE_NAME}.exe"
	  MessageBox MB_ICONSTOP "$(^Name)正在运行，请先关闭程序!"
	  Quit
	${EndIf}
*/

  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "您确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2
  Abort
  
  nsExec::ExecToLog 'taskkill /f /im ClientEngineChild.exe'
  nsExec::ExecToLog 'taskkill /f /im ${EXE_NAME}.exe'

FunctionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从您的计算机移除。"
FunctionEnd
