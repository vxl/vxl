# Microsoft Developer Studio Project File - Name="vcsl" - Package Owner=<4>
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
# vcsl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vcsl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vcsl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vcsl.mak" CFG="vcsl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcsl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vcsl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcsl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vcsl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "vcsl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "vcsl_EXPORTS"
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

# Name "vcsl - Win32 Release"
# Name "vcsl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_angle-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_angle_unit-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_axis-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_cartesian-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_cartesian_2d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_cartesian_3d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_composition-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_coordinate_system-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_cylindrical-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_cylindrical_to_cartesian_3d-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_degree-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_dimension-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_displacement-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_geocentric-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_geodetic-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_geographic-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_graph-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_lambertian-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_length-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_length_unit-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_matrix-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_matrix_param-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_meter-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_perspective-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_polar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_radian-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_rotation-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_scale-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_spatial-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_spatial_transformation-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_spherical-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_spheroid-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_translation-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_unit-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vbl_smart_ptr+vcsl_utm-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcsl_axis_sptr-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcsl_interpolator-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcsl_matrix_param_sptr-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcsl_spatial_sptr-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcsl_spatial_transformation_sptr-.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_angle.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_angle_unit.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_axis.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_2d.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_3d.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_composition.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_coordinate_system.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical_to_cartesian_3d.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_degree.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_dimension.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_displacement.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_geocentric.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_geodetic.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_geographic.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_graph.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_lambertian.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_length.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_length_unit.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_matrix.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_meter.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_perspective.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_polar.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_radian.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_rotation.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_scale.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial_transformation.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_spherical.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_spheroid.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_translation.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_unit.cxx

# End Source File
# Begin Source File

SOURCE=.\vcsl_utm.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\vcsl_angle.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_angle_unit.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_axis.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_2d.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_3d.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_composition.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_coordinate_system.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical_to_cartesian_3d.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_degree.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_dimension.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_displacement.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geocentric.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geodetic.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geographic.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_graph.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_lambertian.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_length.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_length_unit.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_matrix.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_meter.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_perspective.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_polar.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_radian.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_rotation.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_scale.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial_transformation.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spherical.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spheroid.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_translation.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_unit.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_utm.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_angle_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_angle_unit_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_axis_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_2d_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_3d_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cartesian_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_composition_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_coordinate_system_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_cylindrical_to_cartesian_3d_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_degree_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_dimension_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_displacement_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geocentric_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geodetic_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_geographic_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_graph_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_lambertian_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_length_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_length_unit_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_matrix_param.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_matrix_param_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_matrix_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_meter_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_perspective_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_polar_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_radian_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_rotation_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_scale_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spatial_transformation_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spherical_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_spheroid_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_translation_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_unit_sptr.h

# End Source File
# Begin Source File

SOURCE=.\vcsl_utm_sptr.h

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

