# Microsoft Developer Studio Project File - Name="m23d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# $(VXLROOT)/lib/ == override in output directory
# m23d  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=m23d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "m23d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "m23d.mak" CFG="m23d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "m23d - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "m23d - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "m23d - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "m23d_EXPORTS"
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

!ELSEIF  "$(CFG)" == "m23d - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "m23d_EXPORTS"
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

# Name "m23d - Win32 Release"
# Name "m23d - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\m23d_correction_matrix_error.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_make_ortho_projection.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_ortho_flexible_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_ortho_rigid_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_pure_ortho_projection.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_rotation_from_ortho_projection.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_rotation_matrix.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_scaled_ortho_projection.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_select_basis_views.cxx
# End Source File
# Begin Source File
SOURCE=.\m23d_set_q_constraint.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\m23d_correction_matrix_error.h
# End Source File
# Begin Source File
SOURCE=.\m23d_make_ortho_projection.h
# End Source File
# Begin Source File
SOURCE=.\m23d_ortho_flexible_builder.h
# End Source File
# Begin Source File
SOURCE=.\m23d_ortho_rigid_builder.h
# End Source File
# Begin Source File
SOURCE=.\m23d_pure_ortho_projection.h
# End Source File
# Begin Source File
SOURCE=.\m23d_rotation_from_ortho_projection.h
# End Source File
# Begin Source File
SOURCE=.\m23d_rotation_matrix.h
# End Source File
# Begin Source File
SOURCE=.\m23d_scaled_ortho_projection.h
# End Source File
# Begin Source File
SOURCE=.\m23d_select_basis_views.h
# End Source File
# Begin Source File
SOURCE=.\m23d_set_q_constraint.h
# End Source File
# End Group
# End Target
# End Project
