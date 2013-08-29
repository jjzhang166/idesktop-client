!macro InstSuccess

Function .onInstSuccess
  execshell open "$DESKTOP\${SETUP_NAME}.lnk"
FunctionEnd

!macroend
