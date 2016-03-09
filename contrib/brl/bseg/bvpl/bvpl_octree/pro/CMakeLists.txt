# This is contrib/brl/bseg/bvpl/pro/CMakeLists.txt
# bvxm's Process Library

# bprb depends on expat and bvxm_pro depends on bprb primarily
include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)
include_directories( ${BRL_INCLUDE_DIR}/bpro )
include_directories( ${BRL_INCLUDE_DIR}/bpro/bprb )

set(bvpl_octree_pro_sources
    bvpl_octree_processes.h
    bvpl_octree_register.h    bvpl_octree_register.cxx
   )

aux_source_directory(Templates bvpl_octree_pro_sources)
aux_source_directory(processes bvpl_octree_pro_sources)
aux_source_directory(processes/pca bvpl_octree_pro_sources)
aux_source_directory(processes/taylor bvpl_octree_pro_sources)
aux_source_directory(processes/corners bvpl_octree_pro_sources)
aux_source_directory(processes/steerable_filters bvpl_octree_pro_sources)


vxl_add_library(LIBRARY_NAME bvpl_octree_pro LIBRARY_SOURCES ${bvpl_octree_pro_sources})
target_link_libraries(bvpl_octree_pro bvpl_octree_io bvpl_octree bvpl_kernels_io boxm boct boxm_algo_pro boxm_sample boxm_util bprb brdb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl_io)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
