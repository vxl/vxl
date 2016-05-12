# brl/bseg/boxm/util/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
set(boxm_util_sources
    boxm_cell_data_traits.h                 boxm_cell_data_traits.cxx
    boxm_plane_ransac.h
    boxm_quad_scan_iterator.h               boxm_quad_scan_iterator.cxx
    boxm_rational_camera_utils.h            boxm_rational_camera_utils.cxx
    boxm_raytrace_operations.h              boxm_raytrace_operations.cxx
    boxm_triangle_interpolation_iterator.h  boxm_triangle_interpolation_iterator.hxx
    boxm_triangle_scan_iterator.h           boxm_triangle_scan_iterator.cxx
    boxm_utils.h                            boxm_utils.cxx
    boxm_vrml_util.h
    boxm_test_util.h                        boxm_test_util.cxx
   )

aux_source_directory(Templates boxm_util_sources)

vxl_add_library(LIBRARY_NAME boxm_util LIBRARY_SOURCES ${boxm_util_sources})
target_link_libraries(boxm_util boxm boxm_sample bvrml boct bsta ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
