# brl/bseg/boxm/algo/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
set(boxm_algo_sp_sources
    boxm_compute_volume_visibility.h
    boxm_depth_image_triangle.h
    boxm_render_image.h
    boxm_render_image_triangle.h
    boxm_update.h
    boxm_update_triangle.h
    dummy.cxx
   )

add_library(boxm_algo_sp ${boxm_algo_sp_sources})

#processes
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )

target_link_libraries(boxm_algo_sp boxm boxm_util boxm_sample boxm_sample_algo boxm_basic bsta imesh vpgl vgl_algo vgl vil vul vcl)
