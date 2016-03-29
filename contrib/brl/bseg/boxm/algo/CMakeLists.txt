# brl/bseg/boxm/algo/CMakeLists.txt
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${VTHREEP_INCLUDE_DIR}/rply )
set(boxm_algo_sources
    boxm_compute_scene_statistics.h
    boxm_crop_scene.h
    boxm_fill_in_mesh.h
    boxm_fill_internal_cells.h
    boxm_init_scene.h
    boxm_refine.h
    boxm_save_block_raw.h
    boxm_save_scene_raw.h
    boxm_save_scene_raw_general.h
    boxm_upload_mesh.h
    boxm_merge_mog.h                        boxm_merge_mog.cxx
    boxm_split_sample.h
    boxm_compute_scene_difference.h
    boxm_linear_operations.h
    boxm_scene_levels_utils.h
   )

vxl_add_library(LIBRARY_NAME boxm_algo LIBRARY_SOURCES ${boxm_algo_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

target_link_libraries(boxm_algo boxm boxm_util boxm_sample boxm_sample_algo boxm_basic bsta imesh ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl)

#raytracing algorithms
add_subdirectory(rt)

#splatting algorithms
add_subdirectory(sp)

#processes
add_subdirectory(pro)
