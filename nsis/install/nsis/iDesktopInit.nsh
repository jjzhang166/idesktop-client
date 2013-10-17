!macro GetModuleHandle
System::Call `kernel32::GetModuleHandle(i 0) i.R0`
Push $R0
!macroend

!macro iDesktopInit

; ***************** On init *********************************

Var ModuleHandle
Var MyMutex

Var Version
;Var Cancel_Del_Old

Function .onInit

; ******************* is the installer already running? ****

System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME}") i .r1 ?e'
StrCpy $MyMutex $1
Pop $R0
StrCmp $R0 0 +3
MessageBox MB_OK|MB_ICONEXCLAMATION "${SETUP_NAME}安装程序已经运行。"
Quit

/* check  if the app is already install */
StrCpy $9 0
Reinstall_Loop:
  EnumRegKey $1 HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths" $9
  IntOp $9 $9 + 1
  StrCmp $1 '${PRODUCT_NAME}' Reinstall_Next 0
  StrCmp $1 "" Reinstall_End Reinstall_Loop
Reinstall_Next:
  ReadRegStr $1 HKLM '${PRODUCT_UNINST_KEY}' "DisplayVersion"
  ;StrCmp $1 '${PRODUCT_LONGVERSION}' 0 Reinstall_End
  ;MessageBox MB_ICONEXCLAMATION|MB_OK "计算机上已经安装了最新版本，安装程序将退出!"
  ;Quit
  ${VersionCompare} $1 ${PRODUCT_LONGVERSION} $Version  ; $Version=0  版本相同	;$Version=1  版本1较高	;$Version=2  版本2较高
  ${if} $Version == 1
    MessageBox MB_ICONEXCLAMATION|MB_OK "计算机上已经安装了更高版本，安装程序将退出!"
  	Quit
	${elseif} $Version == 2
	  MessageBox MB_ICONEXCLAMATION|MB_OK "请先卸载旧版本，再重新运行新版本安装程序!"
  	Quit
	  ;Goto Reinstall
	${else}
	  MessageBox MB_ICONEXCLAMATION|MB_OK "计算机上已经安装了最新版本，安装程序将退出!"
  	Quit
	${endif}
  
;Reinstall:
	;先卸载旧版本，再安装新版本

	;StrCpy $Cancel_Del_Old 1
  ;ReadRegStr $R6 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString"
  ;StrCmp $R6 "" +4
  ;MessageBox MB_YESNO|MB_ICONEXCLAMATION "是否升级到最新版本？$\n 点击〖是〗将先卸载旧版本.$\n 点击〖否〗退出安装程序." IDYES +2
  ;Abort
  ;ExecWait "$R6 _?=$INSTDIR"
  
  ;MessageBox MB_ICONEXCLAMATION|MB_OK "Cancel_Del_Old == $Cancel_Del_Old"
  
	;${if} $Cancel_Del_Old == 0
  	;Goto Reinstall_End
  ;${else}
  	;MessageBox MB_ICONEXCLAMATION|MB_OK "取消卸载！！！"
    ;Goto Reinstall
	;${endif}
Reinstall_End:

!insertmacro GetModuleHandle
Pop $ModuleHandle


Push $0
Push $1
StrCpy $0 1
System::Call "mscoree::GetCORVersion(w, i ${NSIS_MAX_STRLEN}, *i) i .r1"
StrCmp $1 0 +2
StrCpy $0 0

Pop $1
Exch $0

; ************* Check Windows version ******
    Call WindowsVersionDetection
    Pop $WinVersionLbl

  ; Get parameters
    var /GLOBAL cmdLineParams
    Push $R0
    ${GetParameters} $cmdLineParams

  ; /noad param (without active directory mode)
    var /GLOBAL use_ad
    StrCpy $use_ad "true"

    ClearErrors
    ${GetOptions} $cmdLineParams '/noad' $R0
    IfErrors +2 0
      StrCpy $use_ad "false"

    Pop $R0
  ; to install for all user
    SetShellVarContext all

FunctionEnd

Function un.onInit

System::Call 'kernel32::CreateMutexA(i 0, i 0, t "UN${PRODUCT_NAME}") i .r1 ?e'
StrCpy $MyMutex $1
Pop $R0
StrCmp $R0 0 +3
MessageBox MB_OK|MB_ICONEXCLAMATION "${SETUP_NAME}卸载程序已经运行。"
Quit

/* notify unload */
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 ${SETUP_NAME} ，及其所有的组件？" IDYES +2
  Abort

/* check if the exe is runing and unload notify */
;FindProcDLL::FindProc "${EXE_NAME}.exe"
;  Pop $R0
;  StrCmp $R0 "1" Runing NoRuning
;Runing:
;  MessageBox   MB_OK|MB_ICONEXCLAMATION   "检测到${PRODUCT_NAME}正在运行,卸载无法继续。请在卸载操作前确认本程序已关闭！"
;  Quit
;NoRuning:

CheckRunning:
nsExec::ExecToStack 'tasklist /FI "Imagename eq ${EXE_NAME}.exe" /FO CSV /NH'
Pop $R0 ;return value/error/timeout
Pop $R1 ;printed text, up to ${NSIS_MAX_STRLEN}
;Messagebox mb_ok "$R1"
${WordFind} "$R1" "," "+1" $R2
;Messagebox mb_ok "$R2"
StrCmp '"${EXE_NAME}.exe"' $R2 0 ContinueUninstall
MessageBox   MB_RETRYCANCEL|MB_ICONEXCLAMATION \
'检测到${SETUP_NAME}正在运行,卸载无法继续！\
请关闭${SETUP_NAME}，并单击[重试]。\
或者单击[取消]退出卸载。'  IDRETRY  CheckRunning IDCANCEL 0
Quit
ContinueUninstall:

; Check Windows version
    Call un.WindowsVersionDetection
    Pop $WinVersionLbl

  ; to uninstall for all user
    SetShellVarContext all
FunctionEnd

!macroend
