# Microsoft Developer Studio Project File - Name="vtol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vtol  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vtol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vtol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vtol.mak" CFG="vtol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vtol - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vtol - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vtol - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vtol_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vtol - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vtol_EXPORTS"
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

# Name "vtol - Win32 Release"
# Name "vtol - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx;c"
# Begin Source File
SOURCE=.\Templates/vcl_map+int.vcl_vector+vtol_edge_2d_sptr-~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_map+int.vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_block-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_chain-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_edge-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_edge_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_face-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_face_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_one_chain-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_topology_object-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_two_chain-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_vertex-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_vertex_2d-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vtol_zero_chain-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vtol_chain_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_list+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_block_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_block~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_chain_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_edge_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_edge_2d~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_edge_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_edge~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_face_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_face_2d~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_face_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_face~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_one_chain_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_one_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_topology_object~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_two_chain_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_two_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_vertex_2d_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_vertex_2d~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_vertex_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_vertex~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_zero_chain_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vcl_vector+vtol_zero_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_block.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_chain.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_edge.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_edge_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_face.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_face_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_one_chain.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_cache.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_io.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_object.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_two_chain.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_zero_chain.cxx
# End Source File
# Begin Source File
SOURCE=.\vtol_cycle_processor.cxx
# End Source File
# Begin Source File
SOURCE=.\../vdgl/vdgl_intensity_face.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vdgl_intensity_face-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_block~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_edge~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_face~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_one_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_two_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_vertex~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_zero_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_block~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_edge~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_face~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_one_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_two_chain~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_vertex~-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vtol_list_functions+vtol_zero_chain~-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "txx;h"
# Begin Source File
SOURCE=.\vtol_block.h
# End Source File
# Begin Source File
SOURCE=.\vtol_chain.h
# End Source File
# Begin Source File
SOURCE=.\vtol_edge.h
# End Source File
# Begin Source File
SOURCE=.\vtol_edge_2d.h
# End Source File
# Begin Source File
SOURCE=.\vtol_face.h
# End Source File
# Begin Source File
SOURCE=.\vtol_face_2d.h
# End Source File
# Begin Source File
SOURCE=.\vtol_list_functions.txx
# End Source File
# Begin Source File
SOURCE=.\vtol_list_functions.h
# End Source File
# Begin Source File
SOURCE=.\vtol_one_chain.h
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_cache.h
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_io.h
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_object.h
# End Source File
# Begin Source File
SOURCE=.\vtol_two_chain.h
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex.h
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex_2d.h
# End Source File
# Begin Source File
SOURCE=.\vtol_zero_chain.h
# End Source File
# Begin Source File
SOURCE=.\vtol_block_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_chain_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_dtd.h
# End Source File
# Begin Source File
SOURCE=.\vtol_edge_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_edge_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_face_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_face_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_macros.h
# End Source File
# Begin Source File
SOURCE=.\vtol_one_chain_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_cache_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_topology_object_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_two_chain_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex_2d_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_vertex_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_zero_chain_sptr.h
# End Source File
# Begin Source File
SOURCE=.\vtol_cycle_processor.h
# End Source File
# Begin Source File
SOURCE=.\../vdgl/vdgl_intensity_face.h
# End Source File
# Begin Source File
SOURCE=.\../vdgl/vdgl_intensity_face_sptr.h
# End Source File
# End Group
# End Target
# End Project
