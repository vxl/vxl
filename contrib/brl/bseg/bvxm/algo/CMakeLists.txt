# Sources  for appearance and mixed grid algorithms
include_directories( ${BRL_INCLUDE_DIR}/bbas)
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )

set(bvxm_algo_sources
    bvxm_merge_mog.h         bvxm_merge_mog.cxx
    bvxm_mog_norm.h          bvxm_mog_norm.hxx
    bvxm_create_scene_xml.h  bvxm_create_scene_xml.cxx
   )

aux_source_directory(Templates bvxm_algo_sources)

vxl_add_library(LIBRARY_NAME bvxm_algo LIBRARY_SOURCES ${bvxm_algo_sources})

target_link_libraries( bvxm_algo bvxm bkml bsta bvgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl volm ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vul)



add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
