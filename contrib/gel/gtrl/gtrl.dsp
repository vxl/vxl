# Microsoft Developer Studio Project File - Name="gtrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gtrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gtrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gtrl.mak" CFG="gtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gtrl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gtrl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gtrl - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(IUEROOT)/gel" /I "$(IUEROOT)/vcl/config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "gtrl - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)/gel" /I "$(IUEROOT)/vcl/config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)/vxl" /D "ANSI_DECLARATORS" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "NO_TIMER" /D "TRILIBRARY" /D "_AFXDLL" /D "_MBCS" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../Debug\gtrl.lib"

!ENDIF 

# Begin Target

# Name "gtrl - Win32 Release"
# Name "gtrl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gtrl_netlib_triangle.c
# End Source File
# Begin Source File

SOURCE=.\gtrl_polygon.cxx
# End Source File
# Begin Source File

SOURCE=.\gtrl_triangle.cxx
# End Source File
# Begin Source File

SOURCE=.\gtrl_triangulation.cxx
# End Source File
# Begin Source File

SOURCE=.\gtrl_vertex.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gtrl_triangle-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_smart_ptr+gtrl_vertex-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gtrl_triangle_sptr-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+gtrl_vertex_sptr-.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\gtrl_netlib_triangle.h
# End Source File
# Begin Source File

SOURCE=.\gtrl_polygon.h
# End Source File
# Begin Source File

SOURCE=.\gtrl_triangle.h
# End Source File
# Begin Source File

SOURCE=.\gtrl_triangulation.h
# End Source File
# Begin Source File

SOURCE=.\gtrl_vertex.h
# End Source File
# End Group
# End Target
# End Project
