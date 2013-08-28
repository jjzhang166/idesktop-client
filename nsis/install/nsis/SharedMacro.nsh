!ifndef SHARED_MACRO
!define SHARED_MACRO

!macro AddVCRedist
/*  add vcredist.exe */
  ${If} ${IsWin7}
  ${Else}
  ;Ϊ���ľ������һ������
  System::Call "*(i 0) i .R0"
  ;��Ҫ���ļ������ڴ���
  Push "Installer\Products\6471EC68FFE9C9C3785518AE9830CA43"
  ;����API���м�飬����ֵ��$R1��
  system::call 'Advapi32::RegOpenKey(i ${HKEY_CLASSES_ROOT}, t s, i R0) .iR1'
  ;�رոü��ľ��
  system::call 'Advapi32::RegCloseKey(i R0)'
  ;�ͷ��ڴ�
  system::free $R0
  ;����ֵ��
  ;0���������
  ;2�����������
  ;����ֵ��ʾ����
  ${If} $R1 == 0
;  Messagebox mb_ok "Key Exists!"
  ${Else}
;  Messagebox mb_ok "Key doesn't Exists!"
  InitPluginsDir
  SetOutPath $PLUGINSDIR
  File "..\files\vcredist_x86.exe"
  nsExec::exec "vcredist_x86.exe /q"
  ;Messagebox mb_ok "error"
  ${EndIf}
${EndIf}
!macroend

!macro AddVcRedist_X86
  StrCpy $9 0
VC_X86_Loop:
  EnumRegKey $1 HKCR "Installer\Products" $9
  IntOp $9 $9 + 1
  StrCmp $1 "" VC_X86_Install 0
  ReadRegStr $2 HKCR "Installer\Products\$1" "${PRODUCT_NAME}"
  StrCmp $2 "Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729" VC_X86_Exist VC_X86_Loop ; ;�汾
VC_X86_Install:
  SetOutPath $INSTDIR
  nsExec::exec "regedit /E $TEMP\backupreg_idesktop.reg HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Discardable\PostSetup\ShellNew"
  File "..\files\vcredist_x86.exe"
  nsExec::exec "$INSTDIR\vcredist_x86.exe /q"
  nsExec::exec "regedit.exe /s $TEMP\backupreg_idesktop.reg"
  Delete "$TEMP\backupreg_idesktop.reg"
  goto VC_X86_End
VC_X86_Exist:
  ;Messagebox mb_ok "�Ѿ���װ"
VC_X86_End:
!macroend

!macro AddVcRedist_X64
  IfFileExists $WINDIR\SYSWOW64\*.* VC_X64_Win64 VC_X64_Win32
VC_X64_Win32:
  goto VC_X64_End
VC_X64_Win64:
  StrCpy $9 0
VC_X64_Loop:
  EnumRegKey $1 HKCR "Installer\Products" $9
  IntOp $9 $9 + 1
  StrCmp $1 "" VC_X64_Install 0
  ReadRegStr $2 HKCR "Installer\Products\$1" "ProductName"
  StrCmp $2 "Microsoft Visual C++ 2008 Redistributable - x64 9.0.21022" VC_X64_Exist VC_X64_Loop
VC_X64_Install:
  DetailPrint "���ڰ�װvcredist_x64.exe"
 ; Messagebox mb_ok "���ڰ�װ"
  nsExec::exec "$INSTDIR\vcredist_x64.exe /q"
  goto VC_X64_End
VC_X64_Exist:
  DetailPrint "�Ѿ���װvcredist_x64.exe"
  ;Messagebox mb_ok "�Ѿ���װ"
VC_X64_End:
!macroend
!endif
