# Microsoft Developer Studio Project File - Name="pcbuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CM DSP Header file
# This file is read by the build system of cm, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"  == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# example1  == name of output library
#  /LIBPATH:"$(IUEROOT)/vxl/vcsl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vcsl"  /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl"  /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl"  /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl"  /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl"  /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl"  /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl"  "vcsl.lib" "vnl.lib" "vcl.lib" "vbl.lib" "vcl.lib" "vbl.lib" "vnl.lib" "vcl.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=example1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "example1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "example1.mak" CFG="example1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "example1 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "example1 - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "example1 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "example1 - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "example1 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "example1_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vcsl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vcsl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 "vcsl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "example1 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "example1_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vcsl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vcsl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 "vcsl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "example1 - Win32 MinSizeRel"
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "example1_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vcsl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vcsl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 "vcsl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "example1 - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "example1_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vcsl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vcsl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 "vcsl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "example1 - Win32 Release"
# Name "example1 - Win32 Debug"
# Name "example1 - Win32 MinSizeRel"
# Name "example1 - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=".\CMakeLists.txt"

!IF  "$(CFG)" == "example1 - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"
# Begin Custom Build

"example1.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" "$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vcsl/examples" -O"$(IUEROOT)/vxl/vcsl/examples" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "example1 - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"
# Begin Custom Build

"example1.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" "$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vcsl/examples" -O"$(IUEROOT)/vxl/vcsl/examples" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "example1 - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"
# Begin Custom Build

"example1.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" "$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vcsl/examples" -O"$(IUEROOT)/vxl/vcsl/examples" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "example1 - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vcsl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"
# Begin Custom Build

"example1.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" "$(IUEROOT)/vxl/vcsl/examples/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/vxl/vcsl/examples" -O"$(IUEROOT)/vxl/vcsl/examples" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\example1.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

