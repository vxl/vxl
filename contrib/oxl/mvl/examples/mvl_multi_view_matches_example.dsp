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
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"  == include path
# $(VXLROOT)/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF == compiler defines
# mvl_multi_view_matches_example  == name of output library
#  /LIBPATH:"$(VXLROOT)/lib/$(IntDir)"  /LIBPATH:"$(VXLROOT)/lib/"  /LIBPATH:"$(VXLROOT)/bin/$(IntDir)"  /LIBPATH:"$(VXLROOT)/bin/"  /LIBPATH:"$(VXLROOT)/oxl/mvl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/oxl/mvl"  /LIBPATH:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl"  /LIBPATH:"$(VXLROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vcl"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo"  /LIBPATH:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/netlib"  /LIBPATH:"$(VXLROOT)/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vbl"  /LIBPATH:"$(VXLROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vil"  /LIBPATH:"$(VXLROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vpl"  /LIBPATH:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/jpeg"  /LIBPATH:"$(VXLROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/png"  /LIBPATH:"$(VXLROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/zlib"  /LIBPATH:"$(VXLROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/tiff"  /LIBPATH:"$(VXLROOT)/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vul"  /LIBPATH:"$(VXLROOT)/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vgl"  "opengl32.lib" "glu32.lib" "mvl.lib" "vnl.lib" "vcl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vbl.lib" "vnl_algo.lib" "netlib.lib" "vnl.lib" "vcl.lib" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vul.lib" "vgl.lib" "vcl.lib" "vcl.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=mvl_multi_view_matches_example - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mvl_multi_view_matches_example.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mvl_multi_view_matches_example.mak" CFG="mvl_multi_view_matches_example - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mvl_multi_view_matches_example - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "mvl_multi_view_matches_example - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "mvl_multi_view_matches_example - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "mvl_multi_view_matches_example - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mvl_multi_view_matches_example - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(VXLROOT)/bin/Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "mvl_multi_view_matches_example_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(VXLROOT)/lib/$(IntDir)"  /LIBPATH:"$(VXLROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/bin/$(IntDir)"  /LIBPATH:"$(VXLROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/oxl/mvl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vgl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "mvl_multi_view_matches_example - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$(VXLROOT)/bin/Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "mvl_multi_view_matches_example_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(VXLROOT)/lib/$(IntDir)"  /LIBPATH:"$(VXLROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/bin/$(IntDir)"  /LIBPATH:"$(VXLROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/oxl/mvl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vgl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "mvl_multi_view_matches_example - Win32 MinSizeRel"
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(VXLROOT)/bin/MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "mvl_multi_view_matches_example_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(VXLROOT)/lib/$(IntDir)"  /LIBPATH:"$(VXLROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/bin/$(IntDir)"  /LIBPATH:"$(VXLROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/oxl/mvl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vgl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "mvl_multi_view_matches_example - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$(VXLROOT)/bin/RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF /D "mvl_multi_view_matches_example_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(VXLROOT)/lib/$(IntDir)"  /LIBPATH:"$(VXLROOT)/lib/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/bin/$(IntDir)"  /LIBPATH:"$(VXLROOT)/bin/" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/oxl/mvl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vcl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vnl/algo/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vbl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vil/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vpl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/jpeg/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/png/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/zlib/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/v3p/tiff/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vul/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(VXLROOT)/vxl/vgl/$(OUTDIR)"  /LIBPATH:"$(VXLROOT)/vxl/vgl" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "mvl_multi_view_matches_example - Win32 Release"
# Name "mvl_multi_view_matches_example - Win32 Debug"
# Name "mvl_multi_view_matches_example - Win32 MinSizeRel"
# Name "mvl_multi_view_matches_example - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mvl_multi_view_matches_example.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

