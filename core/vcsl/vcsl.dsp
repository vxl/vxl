# Microsoft Developer Studio Project File - Name="vcsl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

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
!MESSAGE "vcsl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vcsl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)" /I "$(IUEROOT)/vxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
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
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)/vcl" /I "$(IUEROOT)" /I "$(IUEROOT)/vxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vcsl - Win32 Release"
# Name "vcsl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vcsl_angle.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_angle_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_axis.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_cartesian.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_cartesian_2d.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_cartesian_3d.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_composition.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_coordinate_system.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_cylindrical.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_cylindrical_to_cartesian_3d.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_degree.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_dimension.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_displacement.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_geocentric.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_geodetic.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_geographic.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_graph.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_lambertian.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_length.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_length_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_meter.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_perspective.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_polar.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_radian.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_rotation.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_scale.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_spatial.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_spatial_transformation.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_spherical.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_spheroid.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_translation.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vbl_smart_ptr+vcsl_utm.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vcl_vector+vcsl_axis_sptr.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vcl_vector+vcsl_interpolator.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vcl_vector+vcsl_spatial_sptr.cxx
# Begin Source File

# End Source File
SOURCE=.\Templates\vcl_vector+vcsl_spatial_transformation_sptr.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_angle.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_angle_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_axis.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_cartesian.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_cartesian_2d.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_cartesian_3d.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_composition.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_coordinate_system.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_cylindrical.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_cylindrical_to_cartesian_3d.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_degree.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_dimension.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_displacement.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_geocentric.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_geodetic.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_geographic.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_graph.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_lambertian.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_length.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_length_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_meter.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_perspective.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_polar.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_radian.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_rotation.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_scale.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_spatial.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_spatial_transformation.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_spherical.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_spheroid.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_translation.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_unit.cxx
# Begin Source File

# End Source File
SOURCE=.\vcsl_utm.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
