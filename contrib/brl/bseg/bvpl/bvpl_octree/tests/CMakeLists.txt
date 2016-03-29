include_directories( ${MUL_INCLUDE_DIR} )

add_executable( bvpl_octree_test_all
  test_driver.cxx
  test_octree_kernel_operator.cxx
  test_octree_neighbors.cxx
#Test EXHIBITS PATH ERRORS --  FIX_ME
#  test_pca_kernels.cxx
#Test FAILS --  FIX_ME
#  test_taylor_basis.cxx
# Test FAILS FIX_ME
#  test_gauss3D_steerable_filters.cxx
 )

set(bvpl_octree_test_all_sources
    test_utils.h    test_utils.cxx
   )

vxl_add_library(LIBRARY_NAME bvpl_octree_tests_util LIBRARY_SOURCES  ${bvpl_octree_test_all_sources})

target_link_libraries( bvpl_octree_tests_util bvpl_octree_io bvpl_octree bvpl boct boxm ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul)
target_link_libraries( bvpl_octree_test_all bvpl_octree_tests_util bvpl_octree_io bvpl_octree bvpl boct boxm ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul)

add_test( NAME bvpl_octree_test_kernel_operator COMMAND $<TARGET_FILE:bvpl_octree_test_all> test_octree_kernel_operator  )
add_test( NAME bvpl_octree_test_neighbors COMMAND $<TARGET_FILE:bvpl_octree_test_all> test_octree_neighbors  )
#add_test( NAME bvpl_octree_test_pca COMMAND $<TARGET_FILE:bvpl_octree_test_all> test_pca_kernels )
#add_test( NAME bvpl_octree_test_taylor_basis COMMAND $<TARGET_FILE:bvpl_octree_test_all> test_taylor_basis )
#add_test( NAME test_gauss3D_steerable_filters COMMAND $<TARGET_FILE:bvpl_octree_test_all> test_gauss3D_steerable_filters )

add_executable( bvpl_octree_test_include test_include.cxx )
target_link_libraries( bvpl_octree_test_include bvpl_octree)

add_executable( bvpl_octree_test_template_include test_template_include.cxx )
target_link_libraries( bvpl_octree_test_template_include bvpl_octree)

