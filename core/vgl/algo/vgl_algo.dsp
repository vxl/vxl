# Microsoft Developer Studio Project File - Name="vgl_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vgl_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vgl_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vgl_algo.mak" CFG="vgl_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vgl_algo - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vgl_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vgl_algo - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c /Zl
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vgl_algo.lib"

!ELSEIF  "$(CFG)" == "vgl_algo - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /D "WIN32" /D "_DEBUG" /D "_LIB" /FR /FD /GZ /c /Zl
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vgl_algo.lib"

!ENDIF 

# Begin Target

# Name "vgl_algo - Win32 Release"
# Name "vgl_algo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vgl_homg_operators_2d+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vgl_homg_operators_2d+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vgl_homg_operators_3d+double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vgl_homg_operators_3d+float-.cxx
# End Source File
# Begin Source File

SOURCE=.\vgl_algo.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\algo\vgl_homg_operators_2d.h
# End Source File
# Begin Source File

SOURCE=.\algo\vgl_homg_operators_2d.txx
# End Source File
# Begin Source File

SOURCE=.\algo\vgl_homg_operators_3d.h
# End Source File
# Begin Source File

SOURCE=.\algo\vgl_homg_operators_3d.txx
# End Source File
# Begin Source File

SOURCE=.\algo\vgl_rtree.h
# End Source File
# Begin Source File

SOURCE=.\algo\vgl_rtree.txx
# End Source File
# End Target
# End Project
