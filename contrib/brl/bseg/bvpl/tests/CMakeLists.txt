INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

ADD_EXECUTABLE( bvpl_test_all
  test_driver.cxx
  test_subgrid.cxx
  test_neighb_oper.cxx
  test_bvpl_kernels.cxx
  test_bvpl_kernel_functors.cxx
  test_direction_to_color_map.cxx
  test_bvpl_vector_operator.cxx
  test_vector_directions.cxx
  test_detect_corner.cxx
 )

TARGET_LINK_LIBRARIES( bvpl_test_all bvpl bvxm vnl vgl vul vbl testlib)

ADD_TEST( bvpl_test_subgrid                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_subgrid  )
ADD_TEST( bvpl_test_neighb_oper            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_neighb_oper  )
ADD_TEST( bvpl_test_kernels                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_bvpl_kernels  )
ADD_TEST( bvpl_test_bvpl_kernel_functors   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_bvpl_kernel_functors  )
ADD_TEST( bvpl_test_direction_to_color_map ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_direction_to_color_map  )
ADD_TEST( bvpl_test_vector_operator        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_bvpl_vector_operator  )
ADD_TEST( bvpl_test_vector_directions      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_vector_directions  )
if( HACK_FORCE_BRL_FAILING_TESTS )
ADD_TEST( bvpl_test_detect_corner          ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bvpl_test_all test_detect_corner  )
endif()

ADD_EXECUTABLE( bvpl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_test_include bvpl)
ADD_EXECUTABLE( bvpl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bvpl_test_template_include bvpl)
