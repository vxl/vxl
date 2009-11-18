INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( bvpl_octree_test_all
  test_driver.cxx
  test_octree_kernel_operator.cxx
 )

TARGET_LINK_LIBRARIES( bvpl_octree_test_all bvpl_octree bvpl boct boxm testlib vul)

ADD_TEST( test_octree_kernel_operator   ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_octree_kernel_operator  )

ADD_EXECUTABLE( bvpl_octree_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_octree_test_include bvpl_octree)
