# Microsoft Developer Studio Project File - Name="bbgm" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bbgm  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bbgm - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bbgm.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bbgm.mak" CFG="bbgm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bbgm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bbgm - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bbgm - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bbgm_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bbgm - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bbgm_EXPORTS"
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

# Name "bbgm - Win32 Release"
# Name "bbgm - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bbgm_viewer.cxx
# End Source File
# Begin Source File
SOURCE=.\bbgm_image_of.cxx
# End Source File
# Begin Source File
SOURCE=.\bbgm_loader.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_mixture+bsta_num_obs+bsta_gauss_if3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture+bsta_num_obs+bsta_gauss_f1----.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture+bsta_num_obs+bsta_gauss_ff3----.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture+bsta_num_obs+bsta_gauss_if3----.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture+bsta_num_obs+bsta_gauss_sf3----.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_if3-.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bbgm_image_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbgm_image_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbgm_viewer-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+bbgm_image_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vbl_array_2d+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vbl_array_2d+int--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_gaussian_indep+float.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_gaussian_sphere+float.1---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gaussian_indep+float.4--.3---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bbgm_image_of+bsta_num_obs+bsta_mixture_fixed+bsta_num_obs+bsta_gauss_f1-.3---.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bbgm_apply.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_detect.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_image_of.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_image_of.txx
# End Source File
# Begin Source File
SOURCE=.\bbgm_image_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_planes_to_sample.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_update.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_viewer.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_viewer_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_loader.h
# End Source File
# Begin Source File
SOURCE=.\bbgm_measure.h
# End Source File
# End Group
# End Target
# End Project
