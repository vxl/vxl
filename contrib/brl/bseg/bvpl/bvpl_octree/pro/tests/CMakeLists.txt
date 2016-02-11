include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree/pro)

add_executable( bvpl_octree_pro_test_all
  test_driver.cxx
  test_bvpl_plane_propagation_process.cxx
)

target_link_libraries( bvpl_octree_pro_test_all ${VXL_LIB_PREFIX}testlib bvpl_octree_pro bvpl_octree boxm boxm_pro boxm_sample boxm_util boct bvpl bprb brdb sdet ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vcl)

if(SEGFAULT_FIXED)
add_test( NAME bvpl_octree_pro_plane_propagation_process COMMAND $<TARGET_FILE:bvpl_octree_pro_test_all> test_bvpl_plane_propagation_process)
endif()

add_executable( bvpl_octree_pro_test_include test_include.cxx )
target_link_libraries( bvpl_octree_pro_test_include bvpl_octree_pro sdet )
