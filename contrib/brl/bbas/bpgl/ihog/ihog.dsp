# Microsoft Developer Studio Project File - Name="ihog" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# ihog  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ihog - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ihog.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ihog.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ihog - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ihog - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ihog - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "ihog_EXPORTS"
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

!ELSEIF  "$(CFG)" == "ihog - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "ihog_EXPORTS"
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

# Name "ihog - Win32 Release"
# Name "ihog - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\ihog_transform_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_sample_grid_bilin.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_minfo_cost_func.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_lsqr_cost_func.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_cost_func.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_world_roi.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_minimizer.cxx
# End Source File
# Begin Source File
SOURCE=.\ihog_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+ihog_world_roi-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+ihog_transform_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+ihog_transform_2d-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\ihog_minfo_cost_func.h
# End Source File
# Begin Source File
SOURCE=.\ihog_lsqr_cost_func.h
# End Source File
# Begin Source File
SOURCE=.\ihog_image.h
# End Source File
# Begin Source File
SOURCE=.\ihog_sample_grid_bilin.h
# End Source File
# Begin Source File
SOURCE=.\ihog_utils.h
# End Source File
# Begin Source File
SOURCE=.\ihog_transform_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\ihog_transform_2d.h
# End Source File
# Begin Source File
SOURCE=.\ihog_world_roi_sptr.h
# End Source File
# Begin Source File
SOURCE=.\ihog_world_roi.h
# End Source File
# Begin Source File
SOURCE=.\ihog_cost_func.h
# End Source File
# Begin Source File
SOURCE=.\ihog_minimizer.h
# End Source File
# End Group
# End Target
# End Project
