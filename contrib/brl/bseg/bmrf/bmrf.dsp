# Microsoft Developer Studio Project File - Name="bmrf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bmrf  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bmrf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bmrf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bmrf.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bmrf - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bmrf - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bmrf - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bmrf_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bmrf - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bmrf_EXPORTS"
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

# Name "bmrf - Win32 Release"
# Name "bmrf - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bmrf_arc-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bmrf_node-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_arc-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_epi_point-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_epi_seg-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_network-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_node-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+bmrf_epi_point_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+bmrf_epi_seg_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bmrf_network-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_gamma_func-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bmrf_curvel_3d-.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_gamma_func.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_point.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_seg.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_network.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_node.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_network_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_network_builder_params.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_transform.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_epipole.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_curve_3d_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_curvel_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\bmrf_arc.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bmrf_epi_point.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_point_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_seg.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_seg_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_network.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_network_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_node.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_node_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_network_builder.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_network_builder_params.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_epi_transform.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_epipole.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_gamma_func.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_gamma_func_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_curve_3d_builder.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_curvel_3d.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_curvel_3d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_arc.h
# End Source File
# Begin Source File
SOURCE=.\bmrf_arc_sptr.h
# End Source File
# End Group
# End Target
# End Project
