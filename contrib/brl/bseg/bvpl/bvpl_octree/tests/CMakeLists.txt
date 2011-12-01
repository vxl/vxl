INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( bvpl_octree_test_all
  test_driver.cxx
  test_octree_kernel_operator.cxx
  test_octree_neighbors.cxx
  test_pca_kernels.cxx
  test_taylor_basis.cxx
  test_gauss3D_steerable_filters.cxx
 )

SET(bvpl_octree_test_all_sources
    test_utils.h    test_utils.cxx
   )

ADD_LIBRARY(bvpl_octree_tests_util  ${bvpl_octree_test_all_sources})

TARGET_LINK_LIBRARIES( bvpl_octree_test_all bvpl_octree_tests_util bvpl_octree_io bvpl_octree bvpl boct boxm testlib vnl vul)

ADD_TEST( bvpl_octree_test_kernel_operator ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_octree_kernel_operator  )
ADD_TEST( bvpl_octree_test_neighbors       ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_octree_neighbors  )
ADD_TEST( bvpl_octree_test_pca             ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_pca_kernels )
ADD_TEST( bvpl_octree_test_taylor_basis    ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_taylor_basis ) 
ADD_TEST( test_gauss3D_steerable_filters   ${EXECUTABLE_OUTPUT_PATH}/bvpl_octree_test_all test_gauss3D_steerable_filters )

ADD_EXECUTABLE( bvpl_octree_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_octree_test_include bvpl_octree)

ADD_EXECUTABLE( bvpl_octree_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_octree_test_template_include bvpl_octree)

