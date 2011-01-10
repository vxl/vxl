INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvpl/bvpl_octree/pro)

ADD_EXECUTABLE( bvpl_octree_pro_test_all
  test_driver.cxx
  test_bvpl_plane_propagation_process.cxx
)

TARGET_LINK_LIBRARIES( bvpl_octree_pro_test_all testlib bvpl_octree_pro bvpl_octree boxm boxm_pro boxm_sample boxm_util boct bvpl bprb brdb sdet vul vcl)

IF(SEGFAULT_FIXED)
ADD_TEST( bvpl_octree_pro_plane_propagation_process  ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_pro_test_all test_bvpl_plane_propagation_process)
ENDIF(SEGFAULT_FIXED)

ADD_EXECUTABLE( bvpl_octree_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_octree_pro_test_include bvpl_octree_pro sdet )
