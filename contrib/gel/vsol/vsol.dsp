# Microsoft Developer Studio Project File - Name="vsol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vsol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vsol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vsol.mak" CFG="vsol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vsol - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vsol - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vsol - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\gel" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vsol - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\gel" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "vsol - Win32 Release"
# Name "vsol - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vbl_bounding_box_3double-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_box_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_box_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_conic_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_curve_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_curve_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_group_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_group_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_line_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_line_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_point_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_point_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_polygon_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_polygon_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_rectangle_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_rectangle_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_region_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_region_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_spatial_object_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_spatial_object_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_surface_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_triangle_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vsol_triangle_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_list+vsol_spatial_object_2d_sptr.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_list+vsol_spatial_object_3d_sptr.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vsol_point_2d_sptr.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vsol_point_3d_sptr.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vector+vsol_spatial_object_2d~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vector+vsol_spatial_object_3d~-.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_box_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_box_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_conic_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_curve_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_curve_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_group_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_group_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_line_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_line_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_point_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_point_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_polygon_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_polygon_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_rectangle_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_rectangle_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_region_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_region_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_spatial_object_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_spatial_object_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_surface_3d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_triangle_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vsol_triangle_3d.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
