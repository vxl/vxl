#
# ./gel/vsol/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

ADD_LIBRARY(vsol vsol_sources)

SOURCE_FILES(vsol_sources
vsol_box_2d    vsol_point_2d      vsol_spatial_object_2d
vsol_box_3d    vsol_point_3d      vsol_spatial_object_3d
vsol_conic_2d  vsol_polygon_2d    vsol_surface_3d
vsol_curve_2d  vsol_polygon_3d    
# vsol_tetrahedron
#   fails under Win32
vsol_curve_3d  vsol_triangle_2d
# vsol_polyhedron
#   fails under Win32
vsol_group_2d  vsol_rectangle_2d  vsol_triangle_3d
vsol_group_3d  vsol_rectangle_3d  
# vsol_volume_3d
#   fails under Win32
vsol_line_2d   vsol_region_2d
vsol_line_3d   vsol_region_3d
)

AUX_SOURCE_DIRECTORY(Templates vsol_sources)
