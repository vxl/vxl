ADD_LIBRARY( vsol vsol_sources )

SOURCE_FILES( vsol_sources
  vsol_spatial_object_2d
  vsol_box_2d
  vsol_group_2d
  vsol_point_2d
  vsol_curve_2d
  vsol_line_2d
  vsol_conic_2d
  vsol_region_2d
  vsol_polygon_2d
  vsol_triangle_2d
  vsol_rectangle_2d
  vsol_spatial_object_3d
  vsol_box_3d
  vsol_group_3d
  vsol_point_3d
  vsol_curve_3d
  vsol_line_3d
  vsol_surface_3d
  vsol_region_3d
  vsol_polygon_3d
  vsol_triangle_3d
  vsol_rectangle_3d
  vsol_volume_3d
  vsol_polyhedron
  vsol_tetrahedron
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )

AUX_SOURCE_DIRECTORY( Templates vsol_sources )

SUBDIRS( tests examples )
