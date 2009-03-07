# Microsoft Developer Studio Project File - Name="rgtl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# rgtl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rgtl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "rgtl.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "rgtl.mak" CFG="rgtl - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "rgtl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rgtl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rgtl - Win32 Release"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rgtl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "rgtl - Win32 Debug"

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
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rgtl_EXPORTS"
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

# Name "rgtl - Win32 Release"
# Name "rgtl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\rgtl_sqt_base.cxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_array_triangles_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_istream.cxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_object_array_triangles_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_once.cxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_ostream.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_location+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_location+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_location+1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_bounds+1-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_space+2._-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_objects+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_objects+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_point_location+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_location+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_bounds+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_bounds+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_bounds+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_objects+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_geometry+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_geometry+2._-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_cell_geometry+3._-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_location+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_objects+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_object_array_points+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_object_array_points+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_bounds+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_sqt_space+3._-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_cell_geometry+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgtl_octree_point_location+3-.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter "h;hxx;txx"
# Begin Source File
SOURCE=.\rgtl_octree_point_location.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_objects.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_bounds.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_location.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_array_points.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_geometry.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_data_fixed.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_space.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_location.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_objects.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_bounds.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_geometry.txx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_geometry.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_vnl_double_3.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_object_array_triangles_3d.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree_link.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_array_points.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_dual_element.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_tagged_vector.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_array_triangles_3d.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_objects.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_base.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_direction.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_dual.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree_index.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree_data_fixed.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_ostream.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_base.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_separating_axis.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_access.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_stl_vector.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_space.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_array.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_dual_location.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_point_location.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree_policy.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_data_fixed.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_object_array.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_config.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_separating_axis_projection.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_location.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_vnl_vector_fixed.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_tagged_index.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_dual_visit.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_compact_tree_node_data.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_istream.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_closest_polygon_3d.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_bounds.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_static_assert.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_serialize_split.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_dual_index.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_objects.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_bounds.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_meta.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_sqt_cell_location.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_object_once.hxx
# End Source File
# Begin Source File
SOURCE=.\rgtl_octree_cell_geometry.hxx
# End Source File
# End Group
# End Target
# End Project
