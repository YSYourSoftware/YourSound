Name "YourSound"

OutFile "..\..\build\Install YourSound.exe"
BrandingText " "

!define MUI_ICON ".\install.ico"
!define MUI_UNICON ".\uninstall.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP ".\icon-top.bmp"
!define MUI_HEADERIMAGE_UNBITMAP ".\icon-top.bmp"
!define MUI_HEADERIMAGE_RIGHT

!define REG_UNINS "Software\Microsoft\Windows\CurrentVersion\Uninstall\YourSound"
!define REG_DATA "Software\YourSoftware\YourSound"

!include Sections.nsh
!include MUI2.nsh
!include LogicLib.nsh

!include "AssociateFileType.nsh"

!include "Welcome.nsdinc"
!include "Conflicting.nsdinc"
!include "Components.nsdinc"
!include "Paths.nsdinc"
!include "Finish.nsdinc"

!include "UnWelcome.nsdinc"
!include "UnFinish.nsdinc"

!insertmacro MUI_LANGUAGE English

Var VST3Location
Var LIBLocation
Var NewInstall

Section "Core Components" SecCore
	SetRegView 64
	SectionIn RO
	SetOutPath "$INSTDIR"

	File /r "..\..\assets\*"

	File "temp\YourSound.exe"
	File "temp\YSLibMgr.exe"
	File "temp\SDL3.dll"

	CreateDirectory $LIBLocation

	WriteRegStr HKLM "${REG_DATA}" "InstallLocation" $INSTDIR
	WriteRegStr HKLM "${REG_DATA}" "LibraryLocation" $LIBLocation
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentCore" 1

	WriteUninstaller "uninstall.exe"

	WriteRegStr HKLM "${REG_UNINS}" "DisplayName" "YourSound"
	WriteRegStr HKLM "${REG_UNINS}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "${REG_UNINS}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "${REG_UNINS}" "DisplayIcon" "$\"$INSTDIR\icon-short.ico$\""
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
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentStartMenu" 1
	CreateDirectory "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\YourSoftware"
	CreateShortcut "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\YourSoftware\YourSound.lnk" "$INSTDIR\YourSound.exe"
SectionEnd

Section "un.Start Menu Shortcut" UnStartMenu
	Delete "C:\ProgramData\Microsoft\Windows\Start Menu\Programs\YourSoftware\YourSound.lnk"
SectionEnd

Section "VST3 Plugin" SecVST3
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentVST3" 1
SectionEnd

Section "un.VST3 Plugin" UnVST3
SectionEnd

Section "Associate File Types" SecAssociateFiles
	SetRegView 64
	WriteRegDWORD HKLM "${REG_DATA}" "ComponentAssociateFiles" 1

	!insertmacro AssociateFileType \
    	".yslib" \
    	"YourSound Library" \
    	'"$INSTDIR\icon-library.ico"' \
    	'"$INSTDIR\YSLibMgr.exe" install "%1"' \
    	"YourSound.LibraryPackage" \
    	"compressed"
    !insertmacro AssociateFileType \
    	".yspp" \
    	"YourSound Player Preset" \
    	"$INSTDIR\YourSound.exe,1" \
    	'"$INSTDIR\YourSound.exe" --rack-load-player-preset "%1"' \
    	"YourSound.PlayerPresetRackElement" \
    	"document"
    !insertmacro AssociateFileType \
        ".ysrs" \
        "YourSound Rack State" \
        "$INSTDIR\YourSound.exe,1" \
        '"$INSTDIR\YourSound.exe" --rack-load "%1"' \
        "YourSound.RackState" \
        "document"
SectionEnd

Section "un.Associate File Types" UnAssociateFiles
	SetRegView 64
	DeleteRegKey HKCR ".yslib"
	DeleteRegKey HKCR ".yspp"
	DeleteRegKey HKCR ".ysrs"
	DeleteRegKey HKCR "YourSound.LibraryPackage"
	DeleteRegKey HKCR "YourSound.PlayerPresetRackElement"
	DeleteRegKey HKCR "YourSound.RackState"
SectionEnd

Function ComponentsPageLeave
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

	${NSD_GetState} $hCtl_Components_SecAssociateFiles $0
    ${If} $0 == ${BST_CHECKED}
    	SectionSetFlags ${SecAssociateFiles} ${SF_SELECTED}
    ${Else}
    	SectionSetFlags ${SecAssociateFiles} 0
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

	${NSD_GetState} $hCtl_Components_SecAssociateFiles $1
    ${If} $1 == ${BST_CHECKED}
    	SectionGetSize ${SecAssociateFiles} $2
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
	SetRegView 64

	ReadRegStr $INSTDIR HKLM "${REG_DATA}" "InstallLocation"
	StrLen $NewInstall $INSTDIR

	${If} $NewInstall == 0
		Call fnc_Components_Create
		Call ComponentsCheckboxChange
		nsDialogs::Show
	${Else}
		Call fnc_Conflicting_Create
		${NSD_SetText} $hCtl_Conflicting_YSInstallLocation $INSTDIR
		nsDialogs::Show
	${EndIf}
FunctionEnd

Function PathsPageShow
	${If} $NewInstall == 0
		Call fnc_Paths_Create
		nsDialogs::Show
	${EndIf}
FunctionEnd

Function PathsPageLeave
	${If} $NewInstall == 0
		${NSD_GetText} $hCtl_Paths_EXELocationInput_Txt $INSTDIR
		${NSD_GetText} $hCtl_Paths_LIBLocationInput_Txt $LIBLocation
		${NSD_GetText} $hCtl_Paths_VST3LocationInput_Txt $VST3Location
	${EndIf}
FunctionEnd

Function FinishLeave
	${NSD_GetState} $hCtl_Finish_OpenApp $0
	${If} $0 == ${BST_CHECKED}
		ExecShell "open" "$InstDir\YourSound.exe"
	${EndIf}
FunctionEnd

Function un.WelcomeShow

	Call un.fnc_UnWelcome_Show
FunctionEnd

Function un.FinishLeave
	${NSD_GetState} hCtl_UnFinish_TellUs $0
	${If} $0 == ${BST_CHECKED}
		ExecShell "open" "https://YourSoftware.org/projects/YourSound/feedback.html?ctx=uninstall"
	${EndIf}
FunctionEnd

UninstPage custom un.WelcomeShow
UninstPage instfiles
UninstPage custom un.fnc_UnFinish_Show un.FinishLeave

Page custom fnc_Welcome_Show
Page custom ComponentsPageShow ComponentsPageLeave
Page custom PathsPageShow PathsPageLeave
Page instfiles
Page custom fnc_Finish_Show FinishLeave