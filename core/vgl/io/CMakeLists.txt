# vxl/vgl/io/CMakeLists.txt

set(vgl_io_sources
  vgl_io_polygon.cxx vgl_io_polygon.h

  vgl_io_box_2d.txx vgl_io_box_2d.h
  vgl_io_box_3d.txx vgl_io_box_3d.h
  vgl_io_conic.txx vgl_io_conic.h
  vgl_io_homg_line_2d.txx vgl_io_homg_line_2d.h
  vgl_io_homg_line_3d_2_points.txx vgl_io_homg_line_3d_2_points.h
  vgl_io_homg_plane_3d.txx vgl_io_homg_plane_3d.h
  vgl_io_homg_point_1d.txx vgl_io_homg_point_1d.h
  vgl_io_homg_point_2d.txx vgl_io_homg_point_2d.h
  vgl_io_homg_point_3d.txx vgl_io_homg_point_3d.h
  vgl_io_line_2d.txx vgl_io_line_2d.h
  vgl_io_line_3d_2_points.txx vgl_io_line_3d_2_points.h
  vgl_io_line_segment_2d.txx vgl_io_line_segment_2d.h
  vgl_io_line_segment_3d.txx vgl_io_line_segment_3d.h
  vgl_io_plane_3d.txx vgl_io_plane_3d.h
  vgl_io_point_2d.txx vgl_io_point_2d.h
  vgl_io_point_3d.txx vgl_io_point_3d.h
  vgl_io_vector_2d.txx vgl_io_vector_2d.h
  vgl_io_vector_3d.txx vgl_io_vector_3d.h
  vgl_io_cylinder.txx vgl_io_cylinder.h
  vgl_io_infinite_line_3d.txx vgl_io_infinite_line_3d.h
  vgl_io_h_matrix_2d.txx vgl_io_h_matrix_2d.h
)

aux_source_directory(Templates vgl_io_sources)

vxl_add_library(LIBNAME ${VXL_LIB_PREFIX}vgl_io LIBSRCS ${vgl_io_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
