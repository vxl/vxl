# Microsoft Developer Studio Project File - Name="vpgl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
#  == override in output directory
# vpgl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vpgl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "vpgl.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "vpgl.mak" CFG="vpgl - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "vpgl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vpgl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vpgl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vpgl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vpgl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/gel" /I "$(VXLROOT)/contrib/gel/mrc" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "vpgl_EXPORTS"
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
SOURCE=.\Templates/vpgl_affine_camera+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_affine_fundamental_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_calibration_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_fundamental_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_lens_distortion+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_perspective_camera+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.4-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.5-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_poly_radial_distortion+double.6-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_proj_camera+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_radial_distortion+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_reg_fundamental_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+vpgl_proj_camera+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_rational_camera+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_essential_matrix+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_local_rational_camera+double-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+vpgl_camera+double--.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_proj_camera+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_perspective_camera+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_calibration_matrix+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_local_rational_camera+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vpgl_rational_camera+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vsl_binary_loader+vpgl_proj_camera+float--.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;txx"
# Begin Source File
SOURCE=.\vpgl_perspective_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_perspective_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_affine_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_affine_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_affine_fundamental_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_affine_fundamental_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_calibration_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fundamental_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_fundamental_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_lens_distortion.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_lens_distortion.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_poly_radial_distortion.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_proj_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_proj_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_radial_distortion.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_radial_distortion.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_reg_fundamental_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_calibration_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_poly_radial_distortion.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_reg_fundamental_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_rational_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_rational_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_essential_matrix.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_essential_matrix.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_comp_rational_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_comp_rational_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_local_rational_camera.h
# End Source File
# Begin Source File
SOURCE=.\vpgl_local_rational_camera.txx
# End Source File
# Begin Source File
SOURCE=.\vpgl_camera_sptr.h
# End Source File
# End Group
# End Target
# End Project
