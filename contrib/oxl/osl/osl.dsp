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
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl"  == include path
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
!MESSAGE "osl - Win32 MinSizeRel" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 RelWithDebInfo" (based on "Win32 (x86) Static Library")
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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ELSEIF  "$(CFG)" == "osl - Win32 MinSizeRel"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "_MBCS" /D "_ATL_DLL"  /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "osl - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/oxl"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "osl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "osl - Win32 Release"
# Name "osl - Win32 Debug"
# Name "osl - Win32 MinSizeRel"
# Name "osl - Win32 RelWithDebInfo"


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
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

