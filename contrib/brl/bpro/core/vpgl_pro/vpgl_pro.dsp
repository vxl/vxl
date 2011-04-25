# Microsoft Developer Studio Project File - Name="vpgl_pro" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# vpgl_pro  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpgl_pro - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vpgl_pro.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vpgl_pro.mak" CFG="vpgl_pro - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vpgl_pro - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpgl_pro - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpgl_pro - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpgl_pro_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpgl_pro - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vpgl_pro_EXPORTS"
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

# Name "vpgl_pro - Win32 Release"
# Name "vpgl_pro - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vpgl_register.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_load_proj_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_load_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_load_perspective_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_save_perspective_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_load_rational_camera_nitf_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_save_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_nitf_camera_coverage_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_load_local_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_scale_perspective_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_projective_to_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_create_local_rational_camera_nitf_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_create_local_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_create_perspective_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_correct_rational_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_save_perspective_camera_vrml_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_project_process.cxx
# End Source File
# Begin Source File
SOURCE=.\processes/vpgl_convert_to_generic_camera_process.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/brdb_value_t+vpgl_camera_double_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vpgl_register.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_processes.h
# End Source File
# End Group
# End Target
# End Project
