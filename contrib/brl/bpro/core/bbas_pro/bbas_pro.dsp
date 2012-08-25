# Microsoft Developer Studio Project File - Name="bbas_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# bbas_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bbas_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bbas_pro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bbas_pro.mak" CFG="bbas_pro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bbas_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "bbas_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bbas_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bbas_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "bbas_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bbas_pro_EXPORTS"
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

# Name "bbas_pro - Win32 Release"
# Name "bbas_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\bbas_register.cxx
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_float.cxx
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_string.cxx
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_int.cxx
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_unsigned.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_atmospheric_corr_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_string_array_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsta_clone_joint_hist_3d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsta_joint_hist_3d_vrml_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsta_load_joint_hist_3d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsta_save_joint_hist_3d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_merge_string_array_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_camera_angles_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_remove_from_db_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bbas_estimate_irradiance_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/imesh_ply_bbox_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsl_expected_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bsl_fusion_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/bpgl_depth_map_processes.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bbas_1d_array_uint_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bbas_1d_array_unsigned-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbas_1d_array_unsigned-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bbas_1d_array_int_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bbas_1d_array_int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbas_1d_array_int-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbas_1d_array_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bsta_joint_histogram_3d_base_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bbas_1d_array_string-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bbas_1d_array_string_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_io_smart_ptr+bbas_1d_array_float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+bbas_1d_array_float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+bbas_1d_array_float_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+vil_image_view_base_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\bbas_1d_array_float.h
# End Source File
# Begin Source File
SOURCE=.\bbas_register.h
# End Source File
# Begin Source File
SOURCE=.\bbas_processes.h
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_string.h
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_string_sptr.h
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_int.h
# End Source File
# Begin Source File
SOURCE=.\bbas_1d_array_unsigned.h
# End Source File
# End Group
# End Target
# End Project
