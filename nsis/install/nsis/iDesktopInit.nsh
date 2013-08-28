!macro GetModuleHandle
System::Call `kernel32::GetModuleHandle(i 0) i.R0`
Push $R0
!macroend

!macro iDesktopInit

; ***************** On init *********************************

Var ModuleHandle
Var MyMutex
Function .onInit

; ******************* is the installer already running? ****

System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME}") i .r1 ?e'
StrCpy $MyMutex $1
Pop $R0
StrCmp $R0 0 +3
MessageBox MB_OK|MB_ICONEXCLAMATION "${SETUP_NAME}��װ�����Ѿ����С�"
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
  StrCmp $1 '${PRODUCT_LONGVERSION}' 0 Reinstall_End
  MessageBox MB_ICONEXCLAMATION|MB_OK "��������Ѿ���װ�����°汾����װ�����˳�!"
  Quit
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
MessageBox MB_OK|MB_ICONEXCLAMATION "${SETUP_NAME}ж�س����Ѿ����С�"
Quit

/* notify unload */
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "��ȷʵҪ��ȫ�Ƴ� ${SETUP_NAME} ���������е������" IDYES +2
  Abort
  
/* check if the exe is runing and unload notify */
;FindProcDLL::FindProc "${EXE_NAME}.exe"
;  Pop $R0
;  StrCmp $R0 "1" Runing NoRuning
;Runing:
;  MessageBox   MB_OK|MB_ICONEXCLAMATION   "��⵽${PRODUCT_NAME}��������,ж���޷�����������ж�ز���ǰȷ�ϱ������ѹرգ�"
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
'��⵽${SETUP_NAME}��������,ж���޷�������\
��ر�${SETUP_NAME}��������[����]��\
���ߵ���[ȡ��]�˳�ж�ء�'  IDRETRY  CheckRunning IDCANCEL 0
Quit
ContinueUninstall:

; Check Windows version
    Call un.WindowsVersionDetection
    Pop $WinVersionLbl

  ; to uninstall for all user
    SetShellVarContext all
FunctionEnd

!macroend
