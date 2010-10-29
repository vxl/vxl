# Microsoft Developer Studio Project File - Name="bapl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bapl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bapl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bapl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bapl.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bapl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bapl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bapl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bapl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bapl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bapl_EXPORTS"
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

# Name "bapl - Win32 Release"
# Name "bapl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bapl_lowe_pyramid_set.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_affine2d_est.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_keypoint.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_set.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_mi_matcher.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_connectivity.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_cluster.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_affine_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_affine_roi.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_mi_matcher_params.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_bbf_tree.cxx
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_extractor.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bapl_connectivity-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bapl_bbf_node-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bapl_keypoint_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bapl_keypoint_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bapl_lowe_pyramid_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vnl_vector_fixed+double.128-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bapl_lowe_pyramid+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bapl_connectivity-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bapl_keypoint-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# Begin Source File
SOURCE=.\bapl_affine_roi.h
# End Source File
# Begin Source File
SOURCE=.\bapl_mi_matcher.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_pyramid_set_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bapl_affine_transform.h
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_set_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_extractor.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_keypoint.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_keypoint_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint_set.h
# End Source File
# Begin Source File
SOURCE=.\bapl_bbf_tree.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_cluster.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_pyramid.h
# End Source File
# Begin Source File
SOURCE=.\bapl_connectivity_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bapl_affine2d_est.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_pyramid.txx
# End Source File
# Begin Source File
SOURCE=.\bapl_connectivity.h
# End Source File
# Begin Source File
SOURCE=.\bapl_mi_matcher_params.h
# End Source File
# Begin Source File
SOURCE=.\bapl_keypoint.h
# End Source File
# Begin Source File
SOURCE=.\bapl_lowe_pyramid_set.h
# End Source File
# End Group
# End Target
# End Project
