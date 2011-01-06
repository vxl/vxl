INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( bvpl_octree_test_all
  test_driver.cxx
  test_octree_kernel_operator.cxx
  test_octree_neighbors.cxx
  test_pca_kernels.cxx
 )

SET(bvpl_octree_test_all_sources
    test_utils.h    test_utils.cxx
    )

ADD_LIBRARY(bvpl_octree_tests_util  ${bvpl_octree_test_all_sources})

TARGET_LINK_LIBRARIES( bvpl_octree_test_all bvpl_octree_tests_util bvpl_octree bvpl boct boxm testlib vul)

ADD_TEST( bvpl_octree_test_kernel_operator ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_octree_kernel_operator  )
ADD_TEST( bvpl_octree_test_neighbors       ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_octree_neighbors  )
ADD_TEST( bvpl_octree_test_pca             ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_pca_kernels )

ADD_EXECUTABLE( bvpl_octree_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_octree_test_include bvpl_octree)
