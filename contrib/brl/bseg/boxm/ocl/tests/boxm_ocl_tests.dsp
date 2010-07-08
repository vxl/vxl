# Microsoft Developer Studio Project File - Name="boxm_ocl_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# boxm_ocl_tests  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=boxm_ocl_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "boxm_ocl_tests.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "boxm_ocl_tests.mak" CFG="bmrf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "boxm_ocl_tests - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "boxm_ocl_tests - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "boxm_ocl_tests - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_ocl_tests_EXPORTS"
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

!ELSEIF  "$(CFG)" == "boxm_ocl_tests - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/brl/bseg" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "boxm_ocl_tests_EXPORTS"
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

# Name "boxm_ocl_tests - Win32 Release"
# Name "boxm_ocl_tests - Win32 Debug"

# Begin Project
# Begin Target
# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\open_cl_test_data.cxx
# End Source File
# Begin Source File
SOURCE=.\test_octree.cxx
# End Source File
# Begin Source File
SOURCE=.\test_expected_image.cxx
# End Source File
# Begin Source File
SOURCE=.\test_backproject_ray.cxx
# End Source File
# Begin Source File
SOURCE=.\test_ray_bundle.cxx
# End Source File
# Begin Source File
SOURCE=.\test_octree_stack.cxx
# End Source File
# Begin Source File
SOURCE=.\test_octree_image.cxx
# End Source File
# Begin Source File
SOURCE=.\test_statistics.cxx
# End Source File
# Begin Source File
SOURCE=.\test_refine.cxx
# End Source File
# Begin Source File
SOURCE=.\test_update.cxx
# End Source File
# Begin Source File
SOURCE=.\test_file_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_scene_io.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/ray_bundle_test_driver+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/octree_test_driver+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/octree_test_driver+boxm_sample+BOXM_APM_SIMPLE_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/octree_test_driver+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/stat_test_driver+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/stat_test_driver+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+online_update_test_manager+float--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/online_update_test_manager+boxm_sample+BOXM_APM_MOG_GREY--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/bocl_manager+online_update_test_manager+boxm_sample+BOXM_APM_MOG_GREY---.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/online_update_test_manager+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\open_cl_test_data.h
# End Source File
# Begin Source File
SOURCE=.\octree_test_driver.txx
# End Source File
# Begin Source File
SOURCE=.\octree_test_driver.h
# End Source File
# Begin Source File
SOURCE=.\ray_bundle_test_driver.txx
# End Source File
# Begin Source File
SOURCE=.\ray_bundle_test_driver.h
# End Source File
# Begin Source File
SOURCE=.\stat_test_driver.h
# End Source File
# Begin Source File
SOURCE=.\stat_test_driver.txx
# End Source File
# Begin Source File
SOURCE=.\online_update_test_manager.txx
# End Source File
# Begin Source File
SOURCE=.\online_update_test_manager.h
# End Source File
# End Group
# End Target
# End Project
