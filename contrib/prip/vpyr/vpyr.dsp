# Microsoft Developer Studio Project File - Name="vpyr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vpyr  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpyr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpyr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpyr.mak" CFG="vpyr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpyr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vpyr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpyr - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpyr_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vpyr - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpyr_EXPORTS"
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

# Name "vpyr - Win32 Release"
# Name "vpyr - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vpyr_2_pyramid_base.cxx
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_base.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vpyr_2_pyramid.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_pyramid.txx
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_pyramid_base.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_pyramid_level.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_pyramid_level.txx
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_pyramid_level_elts.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid.txx
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_base.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_builder.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_level.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_level.txx
# End Source File
# Begin Source File
SOURCE=.\vpyr_2_tpyramid_level_elts.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_top_tmap.h
# End Source File
# Begin Source File
SOURCE=.\vpyr_top_tmap.txx
# End Source File
# End Group
# End Target
# End Project
