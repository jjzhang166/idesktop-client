!macro InstSuccess

Function .onInstSuccess
	HideWindow
	
	${if} $Bool_Finishpage_Run == 5
  	execshell open "$DESKTOP\${SETUP_NAME}.lnk"
	${endif}

FunctionEnd

!macroend
