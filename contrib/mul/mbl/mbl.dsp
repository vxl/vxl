# Microsoft Developer Studio Project File - Name="mbl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# mbl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mbl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mbl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mbl.mak" CFG="mbl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mbl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "mbl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mbl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mbl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "mbl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/mul"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "mbl_EXPORTS"
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

# Name "mbl - Win32 Release"
# Name "mbl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mbl_screen_counter.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_wrapper+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_array_wrapper+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_array_wrapper+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_collector+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_collector_list+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_wrapper+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_rbf_network.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_stochastic_data_collector+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+mbl_data_collector_base~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_binary_loader+mbl_data_collector_base-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+mbl_chord-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+mbl_stats_1d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vsl_vector_io+mbl_sum_1d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_data_array_ptr_wrapper+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_selected_data_wrapper+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/mbl_selected_data_wrapper+vnl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector_base.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_gamma.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_k_means.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_matrix_products.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_matxvec.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_mz_random.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_read_double.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_read_int.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_read_str.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_read_props.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_stats_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_sum_1d.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_lda.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_thin_plate_spline_2d.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_histogram.cxx

# End Source File
# Begin Source File

SOURCE=.\mbl_clamped_plate_spline_2d.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mbl_data_array_wrapper.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_array_wrapper.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector_base.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector_list.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_collector_list.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_data_wrapper.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_wrapper.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_gamma.h

# End Source File
# Begin Source File

SOURCE=.\mbl_k_means.h

# End Source File
# Begin Source File

SOURCE=.\mbl_lda.h

# End Source File
# Begin Source File

SOURCE=.\mbl_matrix_products.h

# End Source File
# Begin Source File

SOURCE=.\mbl_matxvec.h

# End Source File
# Begin Source File

SOURCE=.\mbl_mz_random.h

# End Source File
# Begin Source File

SOURCE=.\mbl_rbf_network.h

# End Source File
# Begin Source File

SOURCE=.\mbl_read_double.h

# End Source File
# Begin Source File

SOURCE=.\mbl_read_int.h

# End Source File
# Begin Source File

SOURCE=.\mbl_read_str.h

# End Source File
# Begin Source File

SOURCE=.\mbl_read_props.h

# End Source File
# Begin Source File

SOURCE=.\mbl_screen_counter.h

# End Source File
# Begin Source File

SOURCE=.\mbl_stats_1d.h

# End Source File
# Begin Source File

SOURCE=.\mbl_stochastic_data_collector.h

# End Source File
# Begin Source File

SOURCE=.\mbl_stochastic_data_collector.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_sum_1d.h

# End Source File
# Begin Source File

SOURCE=.\mbl_chord.h

# End Source File
# Begin Source File

SOURCE=.\mbl_cloneable_ptr.h

# End Source File
# Begin Source File

SOURCE=.\mbl_index_sort.h

# End Source File
# Begin Source File

SOURCE=.\mbl_lru_cache.h

# End Source File
# Begin Source File

SOURCE=.\mbl_print.h

# End Source File
# Begin Source File

SOURCE=.\mbl_priority_bounded_queue.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_array_ptr_wrapper.h

# End Source File
# Begin Source File

SOURCE=.\mbl_data_array_ptr_wrapper.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_stl.h

# End Source File
# Begin Source File

SOURCE=.\mbl_selected_data_wrapper.h

# End Source File
# Begin Source File

SOURCE=.\mbl_selected_data_wrapper.txx

# End Source File
# Begin Source File

SOURCE=.\mbl_thin_plate_spline_2d.h

# End Source File
# Begin Source File

SOURCE=.\mbl_histogram.h

# End Source File
# Begin Source File

SOURCE=.\mbl_clamped_plate_spline_2d.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

