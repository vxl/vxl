# brl/bseg/boxm2/util/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_util_sources
    boxm_merge_mog.h                        boxm_merge_mog.cxx
    boxm_plane_ransac.h
    boxm_quad_scan_iterator.h               boxm_quad_scan_iterator.cxx
    boxm_rational_camera_utils.h            boxm_rational_camera_utils.cxx
    boxm_raytrace_operations.h              boxm_raytrace_operations.cxx
    boxm_sigma_normalizer.h                 boxm_sigma_normalizer.cxx
    boxm_triangle_interpolation_iterator.h  boxm_triangle_interpolation_iterator.txx
    boxm_triangle_scan_iterator.h           boxm_triangle_scan_iterator.cxx
    boxm_utils.h                            boxm_utils.cxx
    boxm_vrml_util.h
    boxm_test_util.h                        boxm_test_util.cxx
   )


AUX_SOURCE_DIRECTORY(Templates boxm2_util_sources)

ADD_LIBRARY(boxm2_util ${boxm2_util_sources})


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm2_util boxm2 boxm2_sample boct bsta vil vnl vnl_algo vcl vbl vgl vpgl vpgl_algo bvrml)

