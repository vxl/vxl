# gel/vsol/CMakeLists.txt

ADD_LIBRARY(vsol vsol_sources)

SOURCE_FILES(vsol_sources
 vsol_spatial_object_2d.cxx vsol_spatial_object_2d.h vsol_spatial_object_2d_sptr.h
 vsol_box_2d.cxx            vsol_box_2d.h            vsol_box_2d_sptr.h
 vsol_group_2d.cxx          vsol_group_2d.h          vsol_group_2d_sptr.h
 vsol_point_2d.cxx          vsol_point_2d.h          vsol_point_2d_sptr.h
 vsol_curve_2d.cxx          vsol_curve_2d.h          vsol_curve_2d_sptr.h
 vsol_line_2d.cxx           vsol_line_2d.h           vsol_line_2d_sptr.h
 vsol_conic_2d.cxx          vsol_conic_2d.h          vsol_conic_2d_sptr.h
 vsol_region_2d.cxx         vsol_region_2d.h         vsol_region_2d_sptr.h
 vsol_polygon_2d.cxx        vsol_polygon_2d.h        vsol_polygon_2d_sptr.h
 vsol_triangle_2d.cxx       vsol_triangle_2d.h       vsol_triangle_2d_sptr.h
 vsol_rectangle_2d.cxx      vsol_rectangle_2d.h      vsol_rectangle_2d_sptr.h
 vsol_spatial_object_3d.cxx vsol_spatial_object_3d.h vsol_spatial_object_3d_sptr.h
 vsol_box_3d.cxx            vsol_box_3d.h            vsol_box_3d_sptr.h
 vsol_group_3d.cxx          vsol_group_3d.h          vsol_group_3d_sptr.h
 vsol_point_3d.cxx          vsol_point_3d.h          vsol_point_3d_sptr.h
 vsol_curve_3d.cxx          vsol_curve_3d.h          vsol_curve_3d_sptr.h
 vsol_line_3d.cxx           vsol_line_3d.h           vsol_line_3d_sptr.h
 vsol_surface_3d.cxx        vsol_surface_3d.h        vsol_surface_3d_sptr.h
 vsol_region_3d.cxx         vsol_region_3d.h         vsol_region_3d_sptr.h
 vsol_polygon_3d.cxx        vsol_polygon_3d.h        vsol_polygon_3d_sptr.h
 vsol_triangle_3d.cxx       vsol_triangle_3d.h       vsol_triangle_3d_sptr.h
 vsol_rectangle_3d.cxx      vsol_rectangle_3d.h      vsol_rectangle_3d_sptr.h
 vsol_volume_3d.cxx         vsol_volume_3d.h         vsol_volume_3d_sptr.h
 vsol_polyhedron.cxx        vsol_polyhedron.h        vsol_polyhedron_sptr.h
 vsol_tetrahedron.cxx       vsol_tetrahedron.h       vsol_tetrahedron_sptr.h
)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/vxl)

AUX_SOURCE_DIRECTORY(Templates vsol_sources)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
