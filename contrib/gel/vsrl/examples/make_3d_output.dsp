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
# /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"  == include path
# $(IUEROOT)/WIN32-VC60/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
# make_3d_output  == name of output library
#  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl"  "opengl32.lib" "glu32.lib" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vbl.lib" "vnl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vcl.lib" "vsrl.lib" "osl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=make_3d_output - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "make_3d_output.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "make_3d_output.mak" CFG="make_3d_output - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "make_3d_output - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "make_3d_output - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "make_3d_output - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "make_3d_output - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "make_3d_output - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "make_3d_output_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vsrl.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "make_3d_output - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "make_3d_output_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vsrl.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "make_3d_output - Win32 MinSizeRel"
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
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "make_3d_output_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vsrl.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "make_3d_output - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/gel" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "make_3d_output_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/gel/vsrl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/oxl/osl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vsrl.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "make_3d_output - Win32 Release"
# Name "make_3d_output - Win32 Debug"
# Name "make_3d_output - Win32 MinSizeRel"
# Name "make_3d_output - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=".\CMakeLists.txt"

!IF  "$(CFG)" == "make_3d_output - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsrl/CMakeLists.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"
# Begin Custom Build

"make_3d_output.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsrl/examples" -O"$(IUEROOT)/WIN32-VC60/gel/vsrl/examples" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "make_3d_output - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsrl/CMakeLists.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"
# Begin Custom Build

"make_3d_output.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsrl/examples" -O"$(IUEROOT)/WIN32-VC60/gel/vsrl/examples" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "make_3d_output - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsrl/CMakeLists.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"
# Begin Custom Build

"make_3d_output.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsrl/examples" -O"$(IUEROOT)/WIN32-VC60/gel/vsrl/examples" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ELSEIF  "$(CFG)" == "make_3d_output - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/gel/CMakeLists.txt"\
	"$(IUEROOT)/gel/vsrl/CMakeLists.txt"\
	".\CMakeLists.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"
# Begin Custom Build

"make_3d_output.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"cmake.exe" ".\CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/gel/vsrl/examples" -O"$(IUEROOT)/WIN32-VC60/gel/vsrl/examples" -B"$(IUEROOT)/WIN32-VC60"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\make_3d_output.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

