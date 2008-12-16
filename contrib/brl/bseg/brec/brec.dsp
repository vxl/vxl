# Microsoft Developer Studio Project File - Name="brec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brec  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brec.mak" CFG="vil2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brec - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brec - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brec - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brec_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brec - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brec_EXPORTS"
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

# Name "brec - Win32 Release"
# Name "brec - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\brec_hierarchy_edge.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_glitch.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy_detector.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_bg_pair_density.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bgrl2_edge+brec_part_instance-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brec_part_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_fg_pair_density.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bgrl2_graph+brec_part_base.brec_hierarchy_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_fg_bg_pair_density.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bgrl2_vertex+brec_hierarchy_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vgl_rtree+brec_part_instance+vgl_polygon_2d+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brec_part_hierarchy-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bgrl2_edge+brec_part_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_part_base.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_bayesian_propagation.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brec_part_gaussian-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brec_hierarchy_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_part_gaussian.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_pair_density.cxx
# End Source File
# Begin Source File
SOURCE=.\brec_param_estimation.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brec_pair_density.h
# End Source File
# Begin Source File
SOURCE=.\brec_bayesian_propagation.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_gaussian.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_gaussian_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brec_fg_pair_density.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy_builder.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_base.h
# End Source File
# Begin Source File
SOURCE=.\brec_fg_bg_pair_density.h
# End Source File
# Begin Source File
SOURCE=.\brec_glitch.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_base_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brec_hierarchy_edge.h
# End Source File
# Begin Source File
SOURCE=.\brec_bg_pair_density.h
# End Source File
# Begin Source File
SOURCE=.\brec_hierarchy_edge_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brec_part_hierarchy_detector.h
# End Source File
# Begin Source File
SOURCE=.\brec_param_estimation.h
# End Source File
# End Group
# End Target
# End Project
