# Microsoft Developer Studio Project File - Name="vpgl_algo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vpgl_algo  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpgl_algo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vpgl_algo.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vpgl_algo.mak" CFG="vpgl_algo - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vpgl_algo - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpgl_algo - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpgl_algo - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vpgl_algo_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpgl_algo - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vpgl_algo_EXPORTS"
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

# Name "vpgl - Win32 Release"
# Name "vpgl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\vpgl_camera_compute.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_7_point.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_8_point.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_affine_ransac.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_ransac.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_reg_ransac.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_optimize_camera.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_project.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_vsol_lens_warp.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_construct_cameras.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_backproject.cxx
# End Source File
# Begin Source File
SOURCE=.\vpgl_invmap_cost_function.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_list+vgl_h_matrix_2d+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_list+vpgl_proj_camera+double--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File
SOURCE=.\vpgl_camera_compute.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_construct_cameras.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_7_point.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_8_point.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_affine_ransac.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_ransac.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fm_compute_reg_ransac.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_lens_warp_mapper.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_list.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_list.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_optimize_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_project.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_vsol_lens_warp.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_backproject.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_invmap_cost_function.h
# End Source File
# End Group
# End Target
# End Project
