!include x64.nsh
Name "libjpeg-turbo SDK for Visual C++ 64-bit"
OutFile "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}libjpeg-turbo-2.1.4-vc64.exe"
InstallDir "c:\libjpeg-turbo64"

SetCompressor bzip2

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "libjpeg-turbo SDK for Visual C++ 64-bit (required)"
!ifdef WIN64
	${If} ${RunningX64}
	${DisableX64FSRedirection}
	${Endif}
!endif
	SectionIn RO
!ifdef GCC
	IfFileExists $SYSDIR/libturbojpeg.dll exists 0
!else
	IfFileExists $SYSDIR/turbojpeg.dll exists 0
!endif
	goto notexists
	exists:
!ifdef GCC
	MessageBox MB_OK "An existing version of the libjpeg-turbo SDK for Visual C++ 64-bit is already installed.  Please uninstall it first."
!else
	MessageBox MB_OK "An existing version of the libjpeg-turbo SDK for Visual C++ 64-bit or the TurboJPEG SDK is already installed.  Please uninstall it first."
!endif
	quit

	notexists:
	SetOutPath $SYSDIR
!ifdef GCC
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libturbojpeg.dll"
!else
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}turbojpeg.dll"
!endif
	SetOutPath $INSTDIR\bin
!ifdef GCC
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libturbojpeg.dll"
!else
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}turbojpeg.dll"
!endif
!ifdef GCC
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libjpeg-62.dll"
!else
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}jpeg62.dll"
!endif
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}cjpeg.exe"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}djpeg.exe"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}jpegtran.exe"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}tjbench.exe"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}rdjpgcom.exe"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}wrjpgcom.exe"
	SetOutPath $INSTDIR\lib
!ifdef GCC
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libturbojpeg.dll.a"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libturbojpeg.a"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libjpeg.dll.a"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\libjpeg.a"
!else
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}turbojpeg.lib"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}turbojpeg-static.lib"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}jpeg.lib"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\${BUILDDIR}jpeg-static.lib"
!endif
	SetOutPath $INSTDIR\lib\pkgconfig
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\pkgscripts\libjpeg.pc"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\pkgscripts\libturbojpeg.pc"
	SetOutPath $INSTDIR\lib\cmake\libjpeg-turbo
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\pkgscripts\libjpeg-turboConfig.cmake"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\pkgscripts\libjpeg-turboConfigVersion.cmake"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\win\libjpeg-turboTargets.cmake"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\win\libjpeg-turboTargets-release.cmake"
!ifdef JAVA
	SetOutPath $INSTDIR\classes
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\java\turbojpeg.jar"
!endif
	SetOutPath $INSTDIR\include
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src/build\jconfig.h"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\jerror.h"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\jmorecfg.h"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\jpeglib.h"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\turbojpeg.h"
	SetOutPath $INSTDIR\doc
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\README.ijg"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\README.md"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\LICENSE.md"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\example.txt"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\libjpeg.txt"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\structure.txt"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\usage.txt"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\wizard.txt"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\tjexample.c"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\java\TJExample.java"
!ifdef GCC
	SetOutPath $INSTDIR\man\man1
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\cjpeg.1"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\djpeg.1"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\jpegtran.1"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\rdjpgcom.1"
	File "F:/OpenFileOrganizer/libs/libjpeg-turbo-2.1.4-src\wrjpgcom.1"
!endif

	WriteRegStr HKLM "SOFTWARE\libjpeg-turbo64 2.1.4" "Install_Dir" "$INSTDIR"

	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\libjpeg-turbo64 2.1.4" "DisplayName" "libjpeg-turbo SDK v2.1.4 for Visual C++ 64-bit"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\libjpeg-turbo64 2.1.4" "UninstallString" '"$INSTDIR\uninstall_2.1.4.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\libjpeg-turbo64 2.1.4" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\libjpeg-turbo64 2.1.4" "NoRepair" 1
	WriteUninstaller "uninstall_2.1.4.exe"
SectionEnd

Section "Uninstall"
!ifdef WIN64
	${If} ${RunningX64}
	${DisableX64FSRedirection}
	${Endif}
!endif

	SetShellVarContext all

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\libjpeg-turbo64 2.1.4"
	DeleteRegKey HKLM "SOFTWARE\libjpeg-turbo64 2.1.4"

!ifdef GCC
	Delete $INSTDIR\bin\libjpeg-62.dll
	Delete $INSTDIR\bin\libturbojpeg.dll
	Delete $SYSDIR\libturbojpeg.dll
	Delete $INSTDIR\lib\libturbojpeg.dll.a
	Delete $INSTDIR\lib\libturbojpeg.a
	Delete $INSTDIR\lib\libjpeg.dll.a
	Delete $INSTDIR\lib\libjpeg.a
!else
	Delete $INSTDIR\bin\jpeg62.dll
	Delete $INSTDIR\bin\turbojpeg.dll
	Delete $SYSDIR\turbojpeg.dll
	Delete $INSTDIR\lib\jpeg.lib
	Delete $INSTDIR\lib\jpeg-static.lib
	Delete $INSTDIR\lib\turbojpeg.lib
	Delete $INSTDIR\lib\turbojpeg-static.lib
!endif
	Delete $INSTDIR\lib\pkgconfig\libjpeg.pc
	Delete $INSTDIR\lib\pkgconfig\libturbojpeg.pc
	Delete $INSTDIR\lib\cmake\libjpeg-turbo\libjpeg-turboConfig.cmake
	Delete $INSTDIR\lib\cmake\libjpeg-turbo\libjpeg-turboConfigVersion.cmake
	Delete $INSTDIR\lib\cmake\libjpeg-turbo\libjpeg-turboTargets.cmake
	Delete $INSTDIR\lib\cmake\libjpeg-turbo\libjpeg-turboTargets-release.cmake
!ifdef JAVA
	Delete $INSTDIR\classes\turbojpeg.jar
!endif
	Delete $INSTDIR\bin\cjpeg.exe
	Delete $INSTDIR\bin\djpeg.exe
	Delete $INSTDIR\bin\jpegtran.exe
	Delete $INSTDIR\bin\tjbench.exe
	Delete $INSTDIR\bin\rdjpgcom.exe
	Delete $INSTDIR\bin\wrjpgcom.exe
	Delete $INSTDIR\include\jconfig.h
	Delete $INSTDIR\include\jerror.h
	Delete $INSTDIR\include\jmorecfg.h
	Delete $INSTDIR\include\jpeglib.h
	Delete $INSTDIR\include\turbojpeg.h
	Delete $INSTDIR\uninstall_2.1.4.exe
	Delete $INSTDIR\doc\README.ijg
	Delete $INSTDIR\doc\README.md
	Delete $INSTDIR\doc\LICENSE.md
	Delete $INSTDIR\doc\example.txt
	Delete $INSTDIR\doc\libjpeg.txt
	Delete $INSTDIR\doc\structure.txt
	Delete $INSTDIR\doc\usage.txt
	Delete $INSTDIR\doc\wizard.txt
	Delete $INSTDIR\doc\tjexample.c
	Delete $INSTDIR\doc\TJExample.java
!ifdef GCC
	Delete $INSTDIR\man\man1\cjpeg.1
	Delete $INSTDIR\man\man1\djpeg.1
	Delete $INSTDIR\man\man1\jpegtran.1
	Delete $INSTDIR\man\man1\rdjpgcom.1
	Delete $INSTDIR\man\man1\wrjpgcom.1
!endif

	RMDir "$INSTDIR\include"
	RMDir "$INSTDIR\lib\pkgconfig"
	RMDir "$INSTDIR\lib\cmake\libjpeg-turbo"
	RMDir "$INSTDIR\lib\cmake"
	RMDir "$INSTDIR\lib"
	RMDir "$INSTDIR\doc"
!ifdef GCC
	RMDir "$INSTDIR\man\man1"
	RMDir "$INSTDIR\man"
!endif
!ifdef JAVA
	RMDir "$INSTDIR\classes"
!endif
	RMDir "$INSTDIR\bin"
	RMDir "$INSTDIR"

SectionEnd
