# Microsoft Developer Studio Project File - Name="brip" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brip  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brip - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brip.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brip.mak" CFG="brip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brip - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brip - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brip - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brip_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brip - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brip_EXPORTS"
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

# Name "brip - Win32 Release"
# Name "brip - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\brip_vil1_float_ops.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_vil_float_ops.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_para_cvrg.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_para_cvrg_params.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_region_pixel.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_roi.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_watershed.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_watershed_params.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_label_equivalence.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_line_generator.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_rect_mask.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_histogram+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_histogram+vxl_byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_mutual_info+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_mutual_info+vxl_byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brip_region_pixel-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brip_roi-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_max_scale_response+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_quadtree_utils+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+brip_quadtree_node_base-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_blobwise_mutual_info+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_mutual_info+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brip_histogram+float-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brip_vil_ops.h
# End Source File
# Begin Source File
SOURCE=.\brip_histogram.h
# End Source File
# Begin Source File
SOURCE=.\brip_histogram.txx
# End Source File
# Begin Source File
SOURCE=.\brip_mutual_info.h
# End Source File
# Begin Source File
SOURCE=.\brip_mutual_info.txx
# End Source File
# Begin Source File
SOURCE=.\brip_vil1_float_ops.h
# End Source File
# Begin Source File
SOURCE=.\brip_vil_float_ops.h
# End Source File
# Begin Source File
SOURCE=.\brip_para_cvrg.h
# End Source File
# Begin Source File
SOURCE=.\brip_para_cvrg_params.h
# End Source File
# Begin Source File
SOURCE=.\brip_region_pixel.h
# End Source File
# Begin Source File
SOURCE=.\brip_region_pixel_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brip_roi.h
# End Source File
# Begin Source File
SOURCE=.\brip_roi_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brip_watershed.h
# End Source File
# Begin Source File
SOURCE=.\brip_watershed_params.h
# End Source File
# Begin Source File
SOURCE=.\brip_max_scale_response.h
# End Source File
# Begin Source File
SOURCE=.\brip_max_scale_response.txx
# End Source File
# Begin Source File
SOURCE=.\brip_quadtree_node.h
# End Source File
# Begin Source File
SOURCE=.\brip_quadtree_node_base_sptr.h
# End Source File
# Begin Source File
SOURCE=.\brip_quadtree_utils.h
# End Source File
# Begin Source File
SOURCE=.\brip_quadtree_utils.txx
# End Source File
# Begin Source File
SOURCE=.\brip_label_equivalence.h
# End Source File
# Begin Source File
SOURCE=.\brip_line_generator.h
# End Source File
# Begin Source File
SOURCE=.\brip_rect_mask.h
# End Source File
# Begin Source File
SOURCE=.\brip_gaussian_kernel.h
# End Source File
# Begin Source File
SOURCE=.\brip_interp_kernel.h
# End Source File
# Begin Source File
SOURCE=.\brip_kernel.h
# End Source File
# Begin Source File
SOURCE=.\brip_subpix_convolution.h
# End Source File
# Begin Source File
SOURCE=.\brip_blobwise_mutual_info.h
# End Source File
# Begin Source File
SOURCE=.\brip_blobwise_mutual_info.txx
# End Source File
# End Group
# End Target
# End Project
