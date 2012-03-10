# Microsoft Developer Studio Project File - Name="brad_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# brad_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=brad_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "brad_pro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "brad_pro.mak" CFG="brad_pro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "brad_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "brad_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "brad_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brad_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "brad_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "brad_pro_EXPORTS"
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

# Name "brad_pro - Win32 Release"
# Name "brad_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\brad_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_shadow_model_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_phongs_model_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_display_sun_index_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_save_sun_index_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_sun_dir_bin_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_compute_eigenspace_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_load_eigenspace_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_update_joint_hist_3d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_describe_eigenspace_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_save_eigenspace_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_create_eigenspace_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_train_histograms_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_classify_image_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_classify_image_pixel_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_eigenimage_pixel_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_synoptic_function_1d_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_empty_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_radiance_values_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_nitf_abs_radiometric_calibration_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_nitf_read_metadata_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_atmospheric_parameters_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_reflectance_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_save_atmospheric_parameters_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_load_atmospheric_parameters_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_save_image_metadata_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_load_image_metadata_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_estimate_shadows_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_get_sun_angles_date_time_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_get_sun_angles_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_create_atmospheric_parameters_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_convert_reflectance_to_digital_count_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_set_sun_angles_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/brad_create_image_metadata_process.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+brad_eigenspace_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+brad_image_metadata_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+brad_atmospheric_parameters_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\brad_register.h
# End Source File
# Begin Source File
SOURCE=.\brad_processes.h
# End Source File
# End Group
# End Target
# End Project
