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
# /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"  == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL == compiler defines
# vxl_vrml_viewer  == name of output library
#  /LIBPATH:"$(IUEROOT)/v3p/Qv/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/Qv"  /LIBPATH:"$(IUEROOT)/oxl/osl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/osl"  /LIBPATH:"$(IUEROOT)/v3p/netlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/netlib"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo"  /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl"  /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl"  /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl"  /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl"  /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil"  /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil"  /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl"  /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl"  /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg"  /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png"  /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png"  /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib"  /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib"  /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff"  /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff"  /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl"  /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl"  /LIBPATH:"$(IUEROOT)/oxl/mvl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/mvl"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc"  /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui"  /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo"  /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl"  /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl"  /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul"  /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul"  /LIBPATH:"$(IUEROOT)/oxl/vrml/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vrml"  "opengl32.lib" "glu32.lib" "Qv.lib" "osl.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vil.lib" "vpl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "vbl.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vbl.lib" "mvl.lib" "vgui_mfc.lib" "vgui.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vil.lib" "vpl.lib" "vgl_algo.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vgl.lib" "vbl.lib" "vul.lib" "vgl_algo.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vgl.lib" "vil.lib" "vpl.lib" "netlib.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vcl.lib" "vul.lib" "vbl.lib" "vpl.lib" "vcl.lib" "vbl.lib" "vrml.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=vxl_vrml_viewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vxl_vrml_viewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vxl_vrml_viewer.mak" CFG="vxl_vrml_viewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vxl_vrml_viewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "vxl_vrml_viewer - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "vxl_vrml_viewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vxl_vrml_viewer - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vxl_vrml_viewer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/Qv/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/Qv" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/osl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/netlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/mvl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vrml/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vrml" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "Qv.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vgui_mfc.lib"
# ADD LINK32 "vgui.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vrml.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/Qv/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/Qv" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/osl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/netlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/mvl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vrml/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vrml" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "Qv.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vgui_mfc.lib"
# ADD LINK32 "vgui.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vrml.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 MinSizeRel"
# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/Qv/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/Qv" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/osl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/netlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/mvl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vrml/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vrml" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "Qv.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vgui_mfc.lib"
# ADD LINK32 "vgui.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vrml.lib"
# ADD LINK32 /STACK:10000000 


!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vcl/config.win32-VC60" /I "$(IUEROOT)/vxl" /I "$(IUEROOT)/oxl" /I "$(IUEROOT)/v3p" /I "$(IUEROOT)/v3p/jpeg" /I "$(IUEROOT)/v3p/png" /I "$(IUEROOT)/v3p/zlib" /I "$(IUEROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
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

# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/Qv/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/Qv" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/osl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/osl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/netlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/netlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vnl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vnl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vcl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vcl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vil/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vil" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vpl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vpl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/jpeg/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/jpeg" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/png/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/png" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/zlib/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/zlib" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/v3p/tiff/$(IntDir)"  /LIBPATH:"$(IUEROOT)/v3p/tiff" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vbl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vbl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/mvl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/mvl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui/impl/mfc" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vgui/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vgui" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/algo/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl/algo" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vgl/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vgl" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/vxl/vul/$(IntDir)"  /LIBPATH:"$(IUEROOT)/vxl/vul" 
# ADD LINK32 /LIBPATH:"$(IUEROOT)/oxl/vrml/$(IntDir)"  /LIBPATH:"$(IUEROOT)/oxl/vrml" 
# ADD LINK32 "opengl32.lib"
# ADD LINK32 "glu32.lib"
# ADD LINK32 "Qv.lib"
# ADD LINK32 "osl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "jpeg.lib"
# ADD LINK32 "png.lib"
# ADD LINK32 "zlib.lib"
# ADD LINK32 "tiff.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "mvl.lib"
# ADD LINK32 "vgui_mfc.lib"
# ADD LINK32 "vgui.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vgl_algo.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vgl.lib"
# ADD LINK32 "vil.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vnl_algo.lib"
# ADD LINK32 "vnl.lib"
# ADD LINK32 "netlib.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vul.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vpl.lib"
# ADD LINK32 "vcl.lib"
# ADD LINK32 "vbl.lib"
# ADD LINK32 "vrml.lib"
# ADD LINK32 /STACK:10000000 


!ENDIF 

# Begin Target

# Name "vxl_vrml_viewer - Win32 Release"
# Name "vxl_vrml_viewer - Win32 Debug"
# Name "vxl_vrml_viewer - Win32 MinSizeRel"
# Name "vxl_vrml_viewer - Win32 RelWithDebInfo"

# Begin Source File

SOURCE=.\"$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt"

!IF  "$(CFG)" == "vxl_vrml_viewer - Win32 Release"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/osl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/mfc/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vrml/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"vxl_vrml_viewer.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -O"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 Debug"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/osl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/mfc/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vrml/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"vxl_vrml_viewer.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -O"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 MinSizeRel"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/osl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/mfc/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vrml/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"vxl_vrml_viewer.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -O"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -B"$(IUEROOT)"

# End Custom Build

!ELSEIF  "$(CFG)" == "vxl_vrml_viewer - Win32 RelWithDebInfo"
USERDEP__HACK=\
	"$(IUEROOT)/CMakeLists.txt"\
	"$(IUEROOT)/config.cmake/Modules/FindJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeJPEG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativePNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindNativeZLIB.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindOpenGL.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindPNG.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindQv.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindTIFF.cmake"\
	"$(IUEROOT)/config.cmake/Modules/FindZLIB.cmake"\
	"$(IUEROOT)/oxl/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/CMakeLists.txt"\
	"$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt"\
	"$(IUEROOT)/oxl/mvl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/osl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vgui/impl/mfc/CMakeListsLink.txt"\
	"$(IUEROOT)/oxl/vrml/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/jpeg/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/netlib/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/png/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/tiff/CMakeListsLink.txt"\
	"$(IUEROOT)/v3p/zlib/CMakeListsLink.txt"\
	"$(IUEROOT)/vcl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vbl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vgl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vil/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vnl/algo/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vpl/CMakeListsLink.txt"\
	"$(IUEROOT)/vxl/vul/CMakeListsLink.txt"
# Begin Custom Build

"vxl_vrml_viewer.dsp" :  "$(SOURCE)" "$(INTDIR)" "$(OUTDIR)"
	"$(CMAKE_ROOT)/Source/cmake.exe" "$(IUEROOT)/oxl/apps/vxl_vrml_viewer/CMakeLists.txt" -DSP -H"$(IUEROOT)" -S"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -O"$(IUEROOT)/oxl/apps/vxl_vrml_viewer" -B"$(IUEROOT)"

# End Custom Build

!ENDIF

# End Source File
# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\main.cxx

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

