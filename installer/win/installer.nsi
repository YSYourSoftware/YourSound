Name "YourSound"

OutFile "..\..\build\YSInstall.exe"
BrandingText " "

!define MUI_ICON ".\install.ico"
!define MUI_UNICON ".\uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP ".\icon-top.bmp"
!define MUI_HEADERIMAGE_RIGHT

!define REG_UNINS "Software\Microsoft\Windows\CurrentVersion\Uninstall\YourSound"
!define REG_DATA "Software\YourSoftware\YourSound"

!include Sections.nsh
!include MUI2.nsh
!include LogicLib.nsh

!insertmacro MUI_LANGUAGE English

!include "Welcome.nsdinc"
!include "Components.nsdinc"
!include "Paths.nsdinc"
!include "Finish.nsdinc"
!include "UninsWelcome.nsdinc"

Var VST3Location
Var AULocation
Var LIBLocation

Section "Core Components" SecCore
	SetRegView 64
	SectionIn RO
	SetOutPath "$INSTDIR"
	File "YourSound.exe"
	File "..\..\assets\icon.ico"

	WriteRegStr HKLM "${REG_DATA}" "InstallLocation" "$INSTDIR"
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentCore" 1

	WriteUninstaller "uninstall.exe"

	# Uninstall Registry Values
	WriteRegStr HKLM "${REG_UNINS}" "DisplayName" "YourSound"
	WriteRegStr HKLM "${REG_UNINS}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "${REG_UNINS}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "${REG_UNINS}" "DisplayIcon" "$\"$INSTDIR\icon.ico$\""
	WriteRegStr HKLM "${REG_UNINS}" "Publisher" "YourSoftware"
	WriteRegDWORD HKLM "${REG_UNINS}" "Language" $Language
	WriteRegDWORD HKLM "${REG_UNINS}" "NoModify" 1
	WriteRegDWORD HKLM "${REG_UNINS}" "NoRepair" 1
SectionEnd

Section "un.Core Components" UnCore
	SetRegView 64
	DeleteRegKey HKLM "${REG_UNINS}"
	DeleteRegKey HKLM "${REG_DATA}"
	RMDir /r /REBOOTOK "$INSTDIR"
SectionEnd

Section "Start Menu Shortcut" SecStartMenu
	CreateDirectory "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\YourSoftware"
	CreateShortcut "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\YourSoftware\YourSound.lnk" "$INSTDIR\YourSound.exe"
SectionEnd

Section "VST3 Plugin" SecVST3
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentVST3" 1
SectionEnd

Section /o "AU Plugin" SecAU
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentAU" 1
SectionEnd

Section "Factory Library" SecFactory
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentFactory" 1
SectionEnd

Function ComponentsPageLeave
	${NSD_GetState} $hCtl_Components_SecFactory $0
	${If} $0 == ${BST_CHECKED}
		SectionSetFlags ${SecFactory} ${SF_SELECTED}
	${Else}
		SectionSetFlags ${SecFactory} 0
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecAU $0
	${If} $0 == ${BST_CHECKED}
		SectionSetFlags ${SecAU} ${SF_SELECTED}
	${Else}
		SectionSetFlags ${SecAU} 0
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecVST3 $0
	${If} $0 == ${BST_CHECKED}
		SectionSetFlags ${SecVST3} ${SF_SELECTED}
	${Else}
		SectionSetFlags ${SecVST3} 0
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecStartMenu $0
	${If} $0 == ${BST_CHECKED}
		SectionSetFlags ${SecStartMenu} ${SF_SELECTED}
	${Else}
		SectionSetFlags ${SecStartMenu} 0
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecCore $0
	${If} $0 == ${BST_CHECKED}
		SectionSetFlags ${SecCore} ${SF_SELECTED}
	${Else}
		SectionSetFlags ${SecCore} 0
	${EndIf}
FunctionEnd

Function ComponentsCheckboxChange
	StrCpy $0 0

	${NSD_GetState} $hCtl_Components_SecFactory $1
	${If} $1 == ${BST_CHECKED}
		SectionGetSize ${SecFactory} $2
		IntOp $0 $0 + $2
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecAU $1
	${If} $1 == ${BST_CHECKED}
		SectionGetSize ${SecAU} $2
		IntOp $0 $0 + $2
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecVST3 $1
	${If} $1 == ${BST_CHECKED}
		SectionGetSize ${SecVST3} $2
		IntOp $0 $0 + $2
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecStartMenu $1
	${If} $1 == ${BST_CHECKED}
		SectionGetSize ${SecStartMenu} $2
		IntOp $0 $0 + $2
	${EndIf}

	${NSD_GetState} $hCtl_Components_SecCore $1
	${If} $1 == ${BST_CHECKED}
		SectionGetSize ${SecCore} $2
		IntOp $0 $0 + $2
	${EndIf}

	${NSD_SetText} $hCtl_Components_SizeLabel "Space Required:$\r$\n$0 KB"
FunctionEnd

Function ComponentsPageShow
	Call fnc_Components_Create
	Call ComponentsCheckboxChange
	nsDialogs::Show
FunctionEnd

Function PathsPageShow
	Call fnc_Paths_Create
	nsDialogs::Show
FunctionEnd

Function PathsPageLeave
	${NSD_GetText} $hCtl_Paths_EXELocationInput_Txt $INSTDIR
	${NSD_GetText} $hCtl_Paths_LIBLocationInput_Txt $LIBLocation
	${NSD_GetText} $hCtl_Paths_AULocationInput_Txt $VST3Location
	${NSD_GetText} $hCtl_Paths_VST3LocationInput_Txt $AULocation
FunctionEnd

Function FinishLeave
	${NSD_GetState} $hCtl_Finish_OpenApp $0
	${If} $0 == ${BST_CHECKED}
		ExecShell "open" "$InstDir\YourSound.exe"
	${EndIf}
FunctionEnd

#Function un.Welcome
	#Call fnc_UninsWelcome_Show
#FunctionEnd

#UninstPage uninstConfirm un.Welcome /ENABLECANCEL
Page custom fnc_Welcome_Show
Page custom ComponentsPageShow ComponentsPageLeave
Page custom PathsPageShow PathsPageLeave
Page instfiles
Page custom fnc_Finish_Show FinishLeave