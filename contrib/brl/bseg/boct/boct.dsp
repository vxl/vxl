# Microsoft Developer Studio Project File - Name="boct" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boct  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boct - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boct.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boct.mak" CFG="boct - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boct - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boct - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boct - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boct_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boct - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boct_EXPORTS"
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

# Name "boct - Win32 Release"
# Name "boct - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\boct_tree_cell.cxx
# End Source File
# Begin Source File
SOURCE=.\boct_bit_tree.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_loc_code+short-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.vgl_point_3d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_test_util+short.vgl_point_3d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vgl_point_3d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell_reader+short.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.vnl_vector_fixed+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.vnl_vector_fixed+float.2--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree+short.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/boct_tree_cell+short.int-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\boct_test_util.txx
# End Source File
# Begin Source File
SOURCE=.\boct_tree.txx
# End Source File
# Begin Source File
SOURCE=.\boct_loc_code.h
# End Source File
# Begin Source File
SOURCE=.\boct_tree_cell.h
# End Source File
# Begin Source File
SOURCE=.\boct_test_util.h
# End Source File
# Begin Source File
SOURCE=.\boct_tree.h
# End Source File
# Begin Source File
SOURCE=.\boct_tree_cell.txx
# End Source File
# Begin Source File
SOURCE=.\boct_loc_code.txx
# End Source File
# Begin Source File
SOURCE=.\boct_tree_cell_reader.h
# End Source File
# Begin Source File
SOURCE=.\boct_tree_cell_reader.txx
# End Source File
# Begin Source File
SOURCE=.\boct_bit_tree.h
# End Source File
# End Group
# End Target
# End Project
