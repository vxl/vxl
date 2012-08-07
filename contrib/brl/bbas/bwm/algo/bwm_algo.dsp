# Microsoft Developer Studio Project File - Name="bwm_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/brl" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bwm_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bwm_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bwm_algo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bwm_algo.mak" CFG="bwm_algo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bwm_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bwm_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bwm_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/brl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bgui3d_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bwm_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/contrib/brl" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bgui3d_EXPORTS"
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
# Name "bwm_algo - Win32 Release"
# Name "bwm_algo - Win32 Debug"
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bwm_algo.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_image_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_lidar_algo.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_rat_proj_camera.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_soview2D_cross.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_utm_lat_lon.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_delaunay_tri.cxx
# End Source File
# Begin Source File
SOURCE=.\bwm_shape_file.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bwm_algo.h
# End Source File
# Begin Source File
SOURCE=.\bwm_image_processor.h
# End Source File
# Begin Source File
SOURCE=.\bwm_lidar_algo.h
# End Source File
# Begin Source File
SOURCE=.\bwm_rat_proj_camera.h
# End Source File
# Begin Source File
SOURCE=.\bwm_soview2D_cross.h
# End Source File
# Begin Source File
SOURCE=.\bwm_soview2D_vertex.h
# End Source File
# Begin Source File
SOURCE=.\bwm_utils.h
# End Source File
# Begin Source File
SOURCE=.\bwm_utm_lat_lon.h
# End Source File
# Begin Source File
SOURCE=.\bwm_delaunay_tri.h
# End Source File
# Begin Source File
SOURCE=.\bwm_shape_file.h
# End Source File
# End Group
# End Target
# End Project
