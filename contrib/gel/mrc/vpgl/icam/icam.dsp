# Microsoft Developer Studio Project File - Name="icam" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# icam  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=icam - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "icam.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "icam.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "icam - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "icam - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "icam - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "icam_tests_EXPORTS"
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

!ELSEIF  "$(CFG)" == "icam - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "icam_tests_EXPORTS"
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

# Name "icam - Win32 Release"
# Name "icam - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\icam_sample.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_view_metadata.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_view_sphere.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_cost_func.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_depth_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_minimizer.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_depth_trans_pyramid.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_utils.cxx
# End Source File
# Begin Source File
SOURCE=.\icam_transform_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+icam_view_sphere-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsph_view_sphere+vsph_view_point+icam_view_metadata--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_array_3d+vgl_rotation_3d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+icam_view_sphere-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\icam_minimizer.h
# End Source File
# Begin Source File
SOURCE=.\icam_view_metadata.h
# End Source File
# Begin Source File
SOURCE=.\icam_view_sphere.h
# End Source File
# Begin Source File
SOURCE=.\icam_sample.h
# End Source File
# Begin Source File
SOURCE=.\icam_cost_func.h
# End Source File
# Begin Source File
SOURCE=.\icam_depth_transform.h
# End Source File
# Begin Source File
SOURCE=.\icam_depth_trans_pyramid.h
# End Source File
# Begin Source File
SOURCE=.\icam_utils.h
# End Source File
# Begin Source File
SOURCE=.\icam_image.h
# End Source File
# Begin Source File
SOURCE=.\icam_transform_2d.h
# End Source File
# Begin Source File
SOURCE=.\icam_minimizer_params.h
# End Source File
# Begin Source File
SOURCE=.\icam_view_sphere_sptr.h
# End Source File
# End Group
# End Target
# End Project
