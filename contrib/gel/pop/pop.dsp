# Microsoft Developer Studio Project File - Name="pop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -D_AFXDLL -DHAS_OPENGL == compiler defines
#  == override in output directory
# pop  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pop.mak" CFG="pop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pop - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "pop - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pop - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -D_AFXDLL -DHAS_OPENGL /D "pop_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "pop - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -D_AFXDLL -DHAS_OPENGL /D "pop_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "pop - Win32 Release"
# Name "pop - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\pop_edge.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_geometric_cost_function.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_geometric_object.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_graph_cost_function.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_homography_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_object.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_parameter.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_point_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_point_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_projective.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_rigid_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\pop_vertex.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\pop_edge.h
# End Source File
# Begin Source File
SOURCE=.\pop_geometric_cost_function.h
# End Source File
# Begin Source File
SOURCE=.\pop_geometric_object.h
# End Source File
# Begin Source File
SOURCE=.\pop_graph_cost_function.h
# End Source File
# Begin Source File
SOURCE=.\pop_homography_2d.h
# End Source File
# Begin Source File
SOURCE=.\pop_manager.h
# End Source File
# Begin Source File
SOURCE=.\pop_object.h
# End Source File
# Begin Source File
SOURCE=.\pop_parameter.h
# End Source File
# Begin Source File
SOURCE=.\pop_point_2d.h
# End Source File
# Begin Source File
SOURCE=.\pop_point_3d.h
# End Source File
# Begin Source File
SOURCE=.\pop_projective.h
# End Source File
# Begin Source File
SOURCE=.\pop_rigid_3d.h
# End Source File
# Begin Source File
SOURCE=.\pop_transform.h
# End Source File
# Begin Source File
SOURCE=.\pop_vertex.h
# End Source File
# End Group
# End Target
# End Project
