# brl/bseg/boxm/algo/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm_algo_sp_sources
    boxm_compute_volume_visibility.h
    boxm_depth_image_triangle.h
    boxm_render_image.h
    boxm_render_image_triangle.h
    boxm_update.h
    boxm_update_triangle.h
    dummy.cxx
   )

ADD_LIBRARY(boxm_algo_sp ${boxm_algo_sp_sources})

#processes
SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm_algo_sp boxm boxm_util boxm_sample boxm_sample_algo boxm_basic bsta imesh vpgl vgl_algo vgl vil vul vcl)
