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
# /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"  == include path
# $(IUEROOT)/WIN32-VC60/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
# test_vsol_tetrahedron  == name of output library
#  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul"  "opengl32.lib" "glu32.lib" "vsol.lib" "vgl_algo.lib" "vgl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vgl.lib" "vcl.lib" "vnl.lib" "vcl.lib" "vbl.lib" "vul.lib" "vcl.lib" "vgl_algo.lib" "vgl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vbl.lib" "vul.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=test_vsol_tetrahedron - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_vsol_tetrahedron.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_vsol_tetrahedron.mak" CFG="test_vsol_tetrahedron - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_vsol_tetrahedron - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test_vsol_tetrahedron - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "test_vsol_tetrahedron - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "test_vsol_tetrahedron - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_vsol_tetrahedron - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/bin/Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "test_vsol_tetrahedron_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vsol.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/bin/Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "test_vsol_tetrahedron_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vsol.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 MinSizeRel"
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/bin/MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "test_vsol_tetrahedron_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vsol.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(IUEROOT)/WIN32-VC60/bin/RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "test_vsol_tetrahedron_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsol" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vsol.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Group "Source Files"
# Begin Target

# Name "test_vsol_tetrahedron - Win32 Release"
# Name "test_vsol_tetrahedron - Win32 Debug"
# Name "test_vsol_tetrahedron - Win32 MinSizeRel"
# Name "test_vsol_tetrahedron - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=".\CMakeLists.txt"

!IF  "$(CFG)" == "test_vsol_tetrahedron - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeListsLink.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"test_vsol_tetrahedron.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsol/tests" -O"$(IUEROOT)/WIN32-VC60/gel/vsol/tests" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeListsLink.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"test_vsol_tetrahedron.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsol/tests" -O"$(IUEROOT)/WIN32-VC60/gel/vsol/tests" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeListsLink.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"test_vsol_tetrahedron.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsol/tests" -O"$(IUEROOT)/WIN32-VC60/gel/vsol/tests" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "test_vsol_tetrahedron - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsol/CMakeListsLink.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"test_vsol_tetrahedron.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsol/tests" -O"$(IUEROOT)/WIN32-VC60/gel/vsol/tests" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\test_vsol_tetrahedron.cxx

# End Source File
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

