# Microsoft Developer Studio Project File - Name="vsl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vsl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsl.mak" CFG="vsl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vsl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vsl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vsl_EXPORTS"
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

# Name "vsl - Win32 Release"
# Name "vsl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\Templates/vsl_set_io+vcl_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_set_io+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_deque_io+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vsl_binary_loader_base~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_map_io+int.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_map_io+int.vcl_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_map_io+vcl_string.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vcl_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_stack_io+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+vcl_pair+uint.uint-.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+vcl_pair+uint.uint-.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+uint.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+uint.int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+uint.double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+int.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_pair_io+double.uint-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_list_io+int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_list_io+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_complex_io+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_complex_io+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vcl_pair+uint.uint--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+vcl_vector+vcl_pair+uint.uint---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_vector_io+uchar-.cxx
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_io.cxx
# End Source File
# Begin Source File
SOURCE=.\vsl_vector_io_bool.cxx
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_loader_base.cxx
# End Source File
# Begin Source File
SOURCE=.\vsl_indent.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\vsl_binary_explicit_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_loader.h
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_loader.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_binary_loader_base.h
# End Source File
# Begin Source File
SOURCE=.\vsl_clipon_binary_loader.h
# End Source File
# Begin Source File
SOURCE=.\vsl_clipon_binary_loader.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_complex_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_complex_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_deque_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_deque_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_indent.h
# End Source File
# Begin Source File
SOURCE=.\vsl_list_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_list_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_map_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_map_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_pair_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_pair_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_set_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_set_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_stack_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_stack_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_string_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_string_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_vector_io.h
# End Source File
# Begin Source File
SOURCE=.\vsl_vector_io.txx
# End Source File
# Begin Source File
SOURCE=.\vsl_fwd.h
# End Source File
# Begin Source File
SOURCE=.\vsl_quick_file.h
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
