# brl/bseg/boxm/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_util_sources
    boxm_cell_data_traits.h                 boxm_cell_data_traits.cxx
    boxm_plane_ransac.h
    boxm_quad_scan_iterator.h               boxm_quad_scan_iterator.cxx
    boxm_rational_camera_utils.h            boxm_rational_camera_utils.cxx
    boxm_raytrace_operations.h              boxm_raytrace_operations.cxx
    boxm_triangle_interpolation_iterator.h  boxm_triangle_interpolation_iterator.txx
    boxm_triangle_scan_iterator.h           boxm_triangle_scan_iterator.cxx
    boxm_utils.h                            boxm_utils.cxx
    boxm_vrml_util.h
    boxm_test_util.h                        boxm_test_util.cxx
   )

AUX_SOURCE_DIRECTORY(Templates boxm_util_sources)

ADD_LIBRARY(boxm_util ${boxm_util_sources})
TARGET_LINK_LIBRARIES(boxm_util boxm boxm_sample bvrml boct bsta vpgl_algo vnl_algo vgl_algo vpgl vnl_io vil vgl vnl vbl vcl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
