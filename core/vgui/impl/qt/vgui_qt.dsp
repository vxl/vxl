# Microsoft Developer Studio Project File - Name="vgui_qt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui_qt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui_qt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui_qt.mak" CFG="vgui_qt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui_qt - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui_qt - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui_qt - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /I "$(VXLROOT)\oxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vgui_qt.lib"

!ELSEIF  "$(CFG)" == "vgui_qt - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /I "$(VXLROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vgui_qt.lib"

!ENDIF 

# Begin Target

# Name "vgui_qt - Win32 Release"
# Name "vgui_qt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\vgui_qt.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_adaptor.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_dialog_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_menu.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_statusbar.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_tag.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_window.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_statusbar_mocced.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_menu_mocced.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_dialog_impl_mocced.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_adaptor_mocced.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vgui_qt.h
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_adaptor.h
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_dialog_impl.h
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_menu.h
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_statusbar.h
# End Source File
# Begin Source File

SOURCE=.\vgui_qt_window.h
# End Source File
# End Group
# End Target
# End Project
