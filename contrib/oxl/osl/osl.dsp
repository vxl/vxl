# Microsoft Developer Studio Project File - Name="osl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# osl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=osl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "osl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "osl.mak" CFG="osl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "osl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "osl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "osl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
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

# Name "osl - Win32 Release"
# Name "osl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/osl_1d_half_kernel+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/osl_canny_smooth+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/osl_canny_smooth+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/osl_canny_smooth+vil_byte-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/osl_canny_smooth+vil_rgb+uchar--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+osl_edgel_chain~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+osl_edge~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+osl_vertex~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+osl_LINK~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+osl_edge~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+osl_vertex~-.cxx

# End Source File
# Begin Source File

SOURCE=.\internals/droid.cxx

# End Source File
# Begin Source File

SOURCE=.\internals/osl_reorder_chain.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_OrthogRegress.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_break_edge.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_base.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_gradient.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_nms.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox_params.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_port.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell_params.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_canny_smooth.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_chamfer.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_convolve.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_easy_canny.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_edge.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector_params.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_edgel_chain.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_fit_circle.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines_params.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_harris.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_harris_params.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_kernel.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_load_topology.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_ortho_regress.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_roi_window.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_save_topology.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_topology.cxx

# End Source File
# Begin Source File

SOURCE=.\osl_vertex.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\internals/droid.h

# End Source File
# Begin Source File

SOURCE=.\internals/osl_reorder_chain.h

# End Source File
# Begin Source File

SOURCE=.\osl_1d_half_kernel.h

# End Source File
# Begin Source File

SOURCE=.\osl_1d_half_kernel.txx

# End Source File
# Begin Source File

SOURCE=.\osl_OrthogRegress.h

# End Source File
# Begin Source File

SOURCE=.\osl_break_edge.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_base.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_gradient.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_nms.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox_params.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_port.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell_params.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_smooth.h

# End Source File
# Begin Source File

SOURCE=.\osl_canny_smooth.txx

# End Source File
# Begin Source File

SOURCE=.\osl_chamfer.h

# End Source File
# Begin Source File

SOURCE=.\osl_convolve.h

# End Source File
# Begin Source File

SOURCE=.\osl_easy_canny.h

# End Source File
# Begin Source File

SOURCE=.\osl_edge.h

# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector.h

# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector_params.h

# End Source File
# Begin Source File

SOURCE=.\osl_edgel_chain.h

# End Source File
# Begin Source File

SOURCE=.\osl_fit_circle.h

# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines.h

# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines_params.h

# End Source File
# Begin Source File

SOURCE=.\osl_harris.h

# End Source File
# Begin Source File

SOURCE=.\osl_harris_params.h

# End Source File
# Begin Source File

SOURCE=.\osl_kernel.h

# End Source File
# Begin Source File

SOURCE=.\osl_load_topology.h

# End Source File
# Begin Source File

SOURCE=.\osl_ortho_regress.h

# End Source File
# Begin Source File

SOURCE=.\osl_roi_window.h

# End Source File
# Begin Source File

SOURCE=.\osl_save_topology.h

# End Source File
# Begin Source File

SOURCE=.\osl_topology.h

# End Source File
# Begin Source File

SOURCE=.\osl_vertex.h

# End Source File
# Begin Source File

SOURCE=.\osl_hacks.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

