# Microsoft Developer Studio Project File - Name="vxl_vrml_viewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# CM DSP Header file
# This file is read by the build system of cm, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"  == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL == compiler defines
# vxl_vrml_viewer  == name of output library
# /libpath:"$(VXLROOT)/v3p/Qv/$(IntDir)" /libpath:"$(VXLROOT)/oxl/osl/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vnl/$(IntDir)" /libpath:"$(VXLROOT)/vcl/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vnl/algo/$(IntDir)" /libpath:"$(VXLROOT)/v3p/netlib/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vil/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vpl/$(IntDir)" /libpath:"$(VXLROOT)/v3p/jpeg/$(IntDir)" /libpath:"$(VXLROOT)/v3p/png/$(IntDir)" /libpath:"$(VXLROOT)/v3p/zlib/$(IntDir)" /libpath:"$(VXLROOT)/v3p/tiff/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vbl/$(IntDir)" /libpath:"$(VXLROOT)/oxl/mvl/$(IntDir)" /libpath:"$(VXLROOT)/oxl/vgui/impl/mfc/$(IntDir)" /libpath:"$(VXLROOT)/oxl/vgui/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vgl/$(IntDir)" /libpath:"$(VXLROOT)/vxl/vul/$(IntDir)" /libpath:"$(VXLROOT)/oxl/vrml/$(IntDir)"  "opengl32.lib" "glu32.lib" "Qv.lib" "osl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "mvl.lib" "vgui_mfc.lib" "vgui.lib" "vnl_algo.lib" "netlib.lib" "vgl.lib" "vil.lib" "vnl.lib" "vul.lib" "vpl.lib" "vcl.lib" "vbl.lib" "vrml.lib" /STACK:10000000  == libraries linked in 
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
!MESSAGE "vxl_vrml_viewer - Win32 Release" (based on "Win32 (x86) Application")
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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/v3p/Qv/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/osl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/algo/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vil/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vpl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/jpeg/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/png/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/zlib/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/tiff/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vbl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/mvl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vgui/impl/mfc/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vgui/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vgl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vul/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vrml/$(IntDir)"
# ADD LINK32 "opengl32.lib" "glu32.lib" "Qv.lib" "osl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "mvl.lib" "vgui_mfc.lib" "vgui.lib" "vnl_algo.lib" "netlib.lib" "vgl.lib" "vil.lib" "vnl.lib" "vul.lib" "vpl.lib" "vcl.lib" "vbl.lib" "vrml.lib" /STACK:10000000 


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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl" /I "$(VXLROOT)/v3p" /I "$(VXLROOT)/v3p/jpeg" /I "$(VXLROOT)/v3p/png" /I "$(VXLROOT)/v3p/zlib" /I "$(VXLROOT)/v3p/tiff"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL -DHAS_QV -DHAS_JPEG -DHAS_PNG -DHAS_ZLIB -DHAS_TIFF -D_AFXDLL /D "vxl_vrml_viewer_EXPORTS"
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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/v3p/Qv/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/osl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vnl/algo/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vil/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vpl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/jpeg/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/png/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/zlib/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/tiff/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vbl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/mvl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vgui/impl/mfc/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vgui/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vgl/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/vxl/vul/$(IntDir)"
# ADD LINK32 /libpath:"$(VXLROOT)/oxl/vrml/$(IntDir)"
# ADD LINK32 "opengl32.lib" "glu32.lib" "Qv.lib" "osl.lib" "jpeg.lib" "png.lib" "zlib.lib" "tiff.lib" "mvl.lib" "vgui_mfc.lib" "vgui.lib" "vnl_algo.lib" "netlib.lib" "vgl.lib" "vil.lib" "vnl.lib" "vul.lib" "vpl.lib" "vcl.lib" "vbl.lib" "vrml.lib" /STACK:10000000 

!ENDIF 

# Begin Target

# Name "vxl_vrml_viewer - Win32 Release"
# Name "vxl_vrml_viewer - Win32 Debug"

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

