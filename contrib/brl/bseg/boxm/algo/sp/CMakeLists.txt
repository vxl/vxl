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

vxl_add_library(LIBRARY_NAME boxm_algo_sp LIBRARY_SOURCES ${boxm_algo_sp_sources})

#processes
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

target_link_libraries(boxm_algo_sp boxm boxm_util boxm_sample boxm_sample_algo boxm_basic bsta imesh ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)
