ADD_EXECUTABLE( bvpl_test_all
  test_driver.cxx
  test_subgrid.cxx
  test_bvpl_kernels.cxx
 )

TARGET_LINK_LIBRARIES( bvpl_test_all bvpl bvxm testlib vgl vul)

ADD_TEST( bvpl_test_subgrid   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_subgrid  )
ADD_TEST( bvpl_test_kernels   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_kernels  )

ADD_EXECUTABLE( bvpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_test_include bvpl)
