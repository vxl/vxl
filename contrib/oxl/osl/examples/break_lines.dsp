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
# /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"  == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
# break_lines  == name of output library
#  /LIBPATH:"c:/awf/src/vxl/oxl/osl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/oxl/osl"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl"  /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl"  /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib"  /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil"  /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg"  /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png"  /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl"  /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul"  /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul"  "opengl32.lib" "glu32.lib" "osl.lib" "vnl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vbl.lib" "vbl.lib" "vul.lib" "vil.lib" "vpl.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=break_lines - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "break_lines.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "break_lines.mak" CFG="break_lines - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "break_lines - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "break_lines - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "break_lines - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "break_lines - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "break_lines - Win32 Release"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "break_lines_EXPORTS"
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

# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/oxl/osl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/oxl/osl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/netlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "break_lines - Win32 Debug"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "break_lines_EXPORTS"
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

# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/oxl/osl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/oxl/osl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/netlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "break_lines - Win32 MinSizeRel"
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
# ADD CPP /nologo  /D "WIN32" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "break_lines_EXPORTS"
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

# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/oxl/osl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/oxl/osl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/netlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "break_lines - Win32 RelWithDebInfo"

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
# ADD CPP /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/vcl" /I "c:/awf/src/vxl/vcl/config.win32-VC60" /I "c:/awf/src/vxl/vxl" /I "c:/awf/src/vxl/oxl" /I "c:/awf/src/vxl/v3p/jpeg" /I "c:/awf/src/vxl/v3p/png" /I "c:/awf/src/vxl/v3p/zlib" /I "c:/awf/src/vxl/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "break_lines_EXPORTS"
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

# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/oxl/osl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/oxl/osl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vcl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vcl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/netlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/netlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vil/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vil" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vpl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vpl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/jpeg/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/png/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/png" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/zlib/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/zlib" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/v3p/tiff/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/v3p/tiff" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vbl/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vbl" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 /LIBPATH:"c:/awf/src/vxl/vxl/vul/$(IntDir)"  /LIBPATH:"c:/awf/src/vxl/vxl/vul" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "break_lines - Win32 Release"
# Name "break_lines - Win32 Debug"
# Name "break_lines - Win32 MinSizeRel"
# Name "break_lines - Win32 RelWithDebInfo"

# Begin Source File

SOURCE="c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt"

!IF  "$(CFG)" == "break_lines - Win32 Release"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindOpenGL.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/oxl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vcl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vbl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vpl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"break_lines.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/oxl/osl/examples" -O"c:/awf/src/vxl/oxl/osl/examples" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "break_lines - Win32 Debug"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindOpenGL.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/oxl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vcl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vbl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vpl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"break_lines.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/oxl/osl/examples" -O"c:/awf/src/vxl/oxl/osl/examples" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "break_lines - Win32 MinSizeRel"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindOpenGL.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/oxl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vcl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vbl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vpl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"break_lines.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/oxl/osl/examples" -O"c:/awf/src/vxl/oxl/osl/examples" -B"c:/awf/src/vxl"

# End Custom Build

!ELSEIF  "$(CFG)" == "break_lines - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"c:/awf/src/vxl/CMakeLists.txt"\
	"c:/awf/src/vxl/config.cmake/Modules/FindJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeJPEG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativePNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindNativeZLIB.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindOpenGL.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindPNG.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindTIFF.cmake"\
	"c:/awf/src/vxl/config.cmake/Modules/FindZLIB.cmake"\
	"c:/awf/src/vxl/oxl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeLists.txt"\
	"c:/awf/src/vxl/oxl/osl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt"\
	"c:/awf/src/vxl/v3p/jpeg/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/netlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/png/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/tiff/CMakeListsLink.txt"\
	"c:/awf/src/vxl/v3p/zlib/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vcl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vbl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vil/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vnl/algo/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vpl/CMakeListsLink.txt"\
	"c:/awf/src/vxl/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"break_lines.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"c:/awf/src/CMake/Source/cmake.exe" "c:/awf/src/vxl/oxl/osl/examples/CMakeLists.txt" -DSP -H"c:/awf/src/vxl" -S"c:/awf/src/vxl/oxl/osl/examples" -O"c:/awf/src/vxl/oxl/osl/examples" -B"c:/awf/src/vxl"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=c:/awf/src/vxl/oxl/osl/examples/break_lines.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

