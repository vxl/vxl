# brl/bseg/boxm2/algo/CMakeLists.txt
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )
SET(boxm2_algo_sp_sources
    boxm_compute_volume_visibility.h
    boxm_depth_image_triangle.h
    boxm_render_image.h
    boxm_render_image_triangle.h
    boxm_update.h
    boxm_update_triangle.h
    dummy.cxx
   )

ADD_LIBRARY(boxm2_algo_sp ${boxm2_algo_sp_sources}) 

#processes
SUBDIRS(pro)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

TARGET_LINK_LIBRARIES(boxm2_algo_sp boxm2 boxm2_util boxm2_sample boxm2_sample_algo boxm2_basic vcl vgl bsta imesh vpgl bgeo vgl vgl_algo)

