# Microsoft Developer Studio Project File - Name="vmal" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/oxl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vmal  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vmal - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vmal.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vmal.mak" CFG="vmal - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vmal - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vmal - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vmal - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vmal_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vmal - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/oxl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vmal_EXPORTS"
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

# Name "vmal - Win32 Release"
# Name "vmal - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vmal_multi_view_data+vtol_edge_2d_sptr--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vmal_multi_view_data+vtol_vertex_2d_sptr--.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_convert.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_dense_matching.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_homog2d.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_kl.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_kl_params.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_lines_correlation.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_multi_view_data.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_operators.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_rectifier.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_refine_lines.cxx

# End Source File
# Begin Source File

SOURCE=.\vmal_track_lines.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vmal_convert.h

# End Source File
# Begin Source File

SOURCE=.\vmal_dense_matching.h

# End Source File
# Begin Source File

SOURCE=.\vmal_homog2d.h

# End Source File
# Begin Source File

SOURCE=.\vmal_kl.h

# End Source File
# Begin Source File

SOURCE=.\vmal_kl_params.h

# End Source File
# Begin Source File

SOURCE=.\vmal_lines_correlation.h

# End Source File
# Begin Source File

SOURCE=.\vmal_multi_view_data.h

# End Source File
# Begin Source File

SOURCE=.\vmal_operators.h

# End Source File
# Begin Source File

SOURCE=.\vmal_rectifier.h

# End Source File
# Begin Source File

SOURCE=.\vmal_refine_lines.h

# End Source File
# Begin Source File

SOURCE=.\vmal_track_lines.h

# End Source File
# Begin Source File

SOURCE=.\vmal_multi_view_data_edge_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vmal_multi_view_data_vertex_sptr.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

