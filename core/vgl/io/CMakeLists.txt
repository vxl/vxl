# vxl/vgl/io/CMakeLists.txt

set(vgl_io_sources
  vgl_io_polygon.cxx vgl_io_polygon.h

  vgl_io_box_2d.hxx vgl_io_box_2d.h
  vgl_io_box_3d.hxx vgl_io_box_3d.h
  vgl_io_conic.hxx vgl_io_conic.h
  vgl_io_homg_line_2d.hxx vgl_io_homg_line_2d.h
  vgl_io_homg_line_3d_2_points.hxx vgl_io_homg_line_3d_2_points.h
  vgl_io_homg_plane_3d.hxx vgl_io_homg_plane_3d.h
  vgl_io_homg_point_1d.hxx vgl_io_homg_point_1d.h
  vgl_io_homg_point_2d.hxx vgl_io_homg_point_2d.h
  vgl_io_homg_point_3d.hxx vgl_io_homg_point_3d.h
  vgl_io_line_2d.hxx vgl_io_line_2d.h
  vgl_io_line_3d_2_points.hxx vgl_io_line_3d_2_points.h
  vgl_io_line_segment_2d.hxx vgl_io_line_segment_2d.h
  vgl_io_line_segment_3d.hxx vgl_io_line_segment_3d.h
  vgl_io_plane_3d.hxx vgl_io_plane_3d.h
  vgl_io_point_2d.hxx vgl_io_point_2d.h
  vgl_io_point_3d.hxx vgl_io_point_3d.h
  vgl_io_vector_2d.hxx vgl_io_vector_2d.h
  vgl_io_vector_3d.hxx vgl_io_vector_3d.h
  vgl_io_cylinder.hxx vgl_io_cylinder.h
  vgl_io_infinite_line_3d.hxx vgl_io_infinite_line_3d.h
  vgl_io_h_matrix_2d.hxx vgl_io_h_matrix_2d.h
)

aux_source_directory(Templates vgl_io_sources)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vgl_io LIBRARY_SOURCES ${vgl_io_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vsl )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vgl_io-obj OBJECT ${vgl_io_sources})
endif()
