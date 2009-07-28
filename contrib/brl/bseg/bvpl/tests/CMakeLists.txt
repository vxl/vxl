ADD_EXECUTABLE( bvpl_test_all
  test_driver.cxx
  test_subgrid.cxx
  test_neighb_oper.cxx
  test_bvpl_kernels.cxx
  test_bvpl_kernel_functors.cxx
  test_direction_to_color_map.cxx
  test_bvpl_vector_operator.cxx
 )

TARGET_LINK_LIBRARIES( bvpl_test_all bvpl bvxm testlib vgl vul)

ADD_TEST( bvpl_test_subgrid   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_subgrid  )
ADD_TEST( bvpl_test_neighb_oper   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_neighb_oper  )
ADD_TEST( bvpl_test_kernels   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_bvpl_kernels  )
ADD_TEST( bvpl_test_bvpl_kernel_functors   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_bvpl_kernel_functors  )
ADD_TEST( bvpl_test_direction_to_color_map   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_direction_to_color_map  )
ADD_TEST( test_bvpl_vector_operator   ${EXECUTABLE_OUTPUT_PATH}/bvpl_test_all test_bvpl_vector_operator  )

ADD_EXECUTABLE( bvpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_test_include bvpl)
