; mmam.nsi


;--------------------------------

!ifndef VERSION
   !define VERSION Demo
!endif

!macro BIMAGE IMAGE PARMS
	Push $0
	GetTempFileName $0
	File /oname=$0 "${IMAGE}"
	SetBrandingImage ${PARMS} $0
	Delete $0
	Pop $0
!macroend

; The name of the installer
Name "Monster Monster Asteroid Maddness (${VERSION})"
SetCompressor lzma

; The file to write
OutFile "../mmam-${VERSION}-install.exe"

XPStyle on
ShowInstDetails show
SetFont "Showcard Gothic" 9
; SetFont "Gothic I" 9
InstallColors F7FF00 0f0f5a
LicenseBkColor 6F6FEC
AddBrandingImage left 100

; Just to make it three pages...
;SubCaption 0 ": Yet another page..."
;SubCaption 2 ": Yet another page..."
;LicenseText "MMaM License page"
LicenseData "demolic.txt"
;DirText "Lets make a third page!"


;--------------------------------

; Pages

Page license licenseImage
Page components
Page custom customPage
Page directory dirImage
Page instfiles instImage
UninstPage uninstConfirm un.licenseImage
UninstPage instfiles


; The default installation directory
InstallDir $PROGRAMFILES\mmam

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\missingweasel\mmam" "Install_Dir"

;--------------------------------

; The stuff to install
Section "MMaM Exe + Data (required)"

  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; Put file there
  File "..\win-binary\*.*"

  SetOutPath $INSTDIR\doc

  File "..\win-binary\doc\*.*"

  SetOutPath $INSTDIR\doc\pict

  File "..\win-binary\doc\pict\*.*"

  ; Write the installation path into the registry
  WriteRegStr HKLM Software\missingweasel\mmam "Install_Dir" "$INSTDIR"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmam" "DisplayName" "NSIS mmam"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmam" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmam" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmam" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd

Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\MMaM"
  CreateShortCut "$SMPROGRAMS\MMaM\MMaM Help.lnk" "$INSTDIR\doc\mmam.html" "" "$INSTDIR\doc\mmam.html" 0
  CreateShortCut "$SMPROGRAMS\MMaM\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\MMaM\Monster Monster Asteroid Maddenss.lnk" "$INSTDIR\mmam.exe" "" "$INSTDIR\mmam.exe" 0
SectionEnd

Section /o "Desktop Icon"
  CreateShortCut "$WINDIR\desktop\Monster Monster Asteroid Maddenss.lnk" "$INSTDIR\mmam.exe" "" "$INSTDIR\mmam.exe" 0
SectionEnd

Section ""
    MessageBox MB_YESNO "Program Installed.  Run MMaM?" IDNO NoRun
       SetOutPath $INSTDIR
       Exec '"$INSTDIR\mmam.exe"'
    NoRun:
SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mmam"
  DeleteRegKey HKLM SOFTWARE\missingweasel\mmam

  ; Remove files and uninstaller
  Delete $INSTDIR\mmam.exe
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\sprites.dat
  Delete $INSTDIR\*.*
  Delete "$INSTDIR\doc\*.*"
  Delete "$INSTDIR\doc\pict\*.*"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\mmam\*.*"
  Delete "$WINDIR\desktop\Monster Monster Asteroid Maddenss.lnk"

  ; Remove directories used
  RMDir "$INSTDIR\doc\pict"
  RMDir "$INSTDIR\doc"
  RMDir "$INSTDIR"
  RMDir "$SMPROGRAMS\mmam"

SectionEnd

;--------------------------------

Section ""
SectionEnd

;--------------------------------

Function licenseImage
	!insertmacro BIMAGE "installer.bmp" /RESIZETOFIT
;	!insertmacro BIMAGE "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" /RESIZETOFIT
FunctionEnd

Function un.licenseImage
	!insertmacro BIMAGE "installer.bmp" /RESIZETOFIT
;	!insertmacro BIMAGE "${NSISDIR}\Contrib\Graphics\Header\nsis.bmp" /RESIZETOFIT
FunctionEnd

Function customPage
FunctionEnd

Function dirImage
FunctionEnd

Function instImage
FunctionEnd


