# Microsoft Developer Studio Project File - Name="vgui_vrml" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgui_vrml - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgui_vrml.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgui_vrml.mak" CFG="vgui_vrml - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgui_vrml - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgui_vrml - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgui_vrml - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\v3p" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vgui_vrml.lib"

!ELSEIF  "$(CFG)" == "vgui_vrml - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\v3p" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vgui_vrml.lib"

!ENDIF 

# Begin Target

# Name "vgui_vrml - Win32 Release"
# Name "vgui_vrml - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vcl_vector+QvNode~-.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+vgui_vrml_texture_map~-.cxx"
# End Source File
# Begin Source File

SOURCE=.\vgui_vrml_draw_visitor.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_vrml_tableau.cxx
# End Source File
# Begin Source File

SOURCE=.\vgui_vrml_texture_map.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+point2D_const~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+point3D~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vector3D_const~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vgui_vrml_tableau_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\vgui_vrml_draw_visitor.h
# End Source File
# Begin Source File

SOURCE=.\vgui_vrml_tableau.h
# End Source File
# Begin Source File

SOURCE=.\vgui_vrml_texture_map.h
# End Source File
# End Group
# End Target
# End Project
