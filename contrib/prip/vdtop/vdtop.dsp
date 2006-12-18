# Microsoft Developer Studio Project File - Name="vdtop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vdtop  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vdtop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vdtop.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vdtop.mak" CFG="vdtop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vdtop - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vdtop - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vdtop - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vdtop_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vdtop - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/prip" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vdtop_EXPORTS"
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

# Name "vdtop - Win32 Release"
# Name "vdtop - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vdtop_8_neighborhood_mask.cxx
# End Source File
# Begin Source File
SOURCE=.\vdtop_freeman_code.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vdtop_4_lower_homotopic_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_4_lower_leveling_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_4_upper_homotopic_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_4_upper_leveling_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_8_lower_homotopic_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_8_lower_leveling_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_8_neighborhood_mask.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_8_upper_homotopic_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_8_upper_leveling_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_construct_well_composed.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_draw_digital_graph.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_freeman_code.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_misc.txx
# End Source File
# Begin Source File
SOURCE=.\vdtop_neighborhood.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_pixel.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_pixel.txx
# End Source File
# Begin Source File
SOURCE=.\vdtop_replace_quasi_8_minima.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_4_veinerization_structure.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_4_veinerization_structure.txx
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_8_veinerization_structure.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_8_veinerization_structure.txx
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_structure_from_digital_graph.h
# End Source File
# Begin Source File
SOURCE=.\vdtop_set_structure_from_digital_graph.txx
# End Source File
# Begin Source File
SOURCE=.\vdtop_well_composed_lower_leveling_kernel.h
# End Source File
# Begin Source File
SOURCE=.\vil_canny_deriche_grad_filter.h
# End Source File
# End Group
# End Target
# End Project
