# ./gel/vtol/CMakeLists.txt

ADD_LIBRARY( vsol vsol_sources )

SOURCE_FILES( vsol_sources
  vsol_spatial_object_2d.cxx
  vsol_box_2d.cxx
  vsol_group_2d.cxx
  vsol_point_2d.cxx
  vsol_curve_2d.cxx
  vsol_line_2d.cxx
  vsol_conic_2d.cxx
  vsol_region_2d.cxx
  vsol_polygon_2d.cxx
  vsol_triangle_2d.cxx
  vsol_rectangle_2d.cxx
  vsol_spatial_object_3d.cxx
  vsol_box_3d.cxx
  vsol_group_3d.cxx
  vsol_point_3d.cxx
  vsol_curve_3d.cxx
  vsol_line_3d.cxx
  vsol_surface_3d.cxx
  vsol_region_3d.cxx
  vsol_polygon_3d.cxx
  vsol_triangle_3d.cxx
  vsol_rectangle_3d.cxx
  vsol_volume_3d.cxx
  vsol_polyhedron.cxx
  vsol_tetrahedron.cxx
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

AUX_SOURCE_DIRECTORY( Templates vsol_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vsol/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
