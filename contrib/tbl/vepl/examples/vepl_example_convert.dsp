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
# /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"  == include path
# $(IUEROOT)/WIN32-VC60/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
# vepl_example_convert  == name of output library
#  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/lib/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/$(IntDir)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/bin/"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vil"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/vxl/vpl"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/png"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/zlib"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/v3p/tiff"  "vepl.lib" "vipl.lib" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=vepl_example_convert - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vepl_example_convert.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vepl_example_convert.mak" CFG="vepl_example_convert - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vepl_example_convert - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "vepl_example_convert - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "vepl_example_convert - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vepl_example_convert - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vepl_example_convert - Win32 Release"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vepl_example_convert_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
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
# ADD LINK32 "vepl.lib"
# ADD LINK32 "vipl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vepl_example_convert - Win32 Debug"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vepl_example_convert_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
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
# ADD LINK32 "vepl.lib"
# ADD LINK32 "vipl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vepl_example_convert - Win32 MinSizeRel"
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
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vepl_example_convert_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
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
# ADD LINK32 "vepl.lib"
# ADD LINK32 "vipl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vepl_example_convert - Win32 RelWithDebInfo"

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
# ADD CPP /I "$(IUEROOT)/WIN32-VC60/vcl" /I "$(IUEROOT)/WIN32-VC60/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/tbl" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "vepl_example_convert_EXPORTS"
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
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vepl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl/$(OUTDIR)"  /LIBPATH:"$(IUEROOT)/WIN32-VC60/tbl/vipl" 
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
# ADD LINK32 "vepl.lib"
# ADD LINK32 "vipl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "vepl_example_convert - Win32 Release"
# Name "vepl_example_convert - Win32 Debug"
# Name "vepl_example_convert - Win32 MinSizeRel"
# Name "vepl_example_convert - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\example_convert.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

