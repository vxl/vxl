# Microsoft Developer Studio Project File - Name="vgui_glut" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui_glut - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui_glut.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui_glut.mak" CFG="vgui_glut - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui_glut - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui_glut - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui_glut - Win32 Release"

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
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vgui_glut.lib"

!ELSEIF  "$(CFG)" == "vgui_glut - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(VXLROOT)\vcl\config.win32-vc60" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vxl" /I "$(VXLROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vgui_glut.lib"

!ENDIF 

# Begin Target

# Name "vgui_glut - Win32 Release"
# Name "vgui_glut - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\menu_hack_none.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vgui_glut_adaptor~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vgui_glut_menu_hack+-per_window_record~-.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_adaptor.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_popup_impl.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_slab.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_tag.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_window.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\menu_hack.h
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_impl.h
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_adaptor.h
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_popup_impl.h
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_slab.h
# End Source File
# Begin Source File

SOURCE=.\vgui_glut_window.h
# End Source File
# End Group
# End Target
# End Project
