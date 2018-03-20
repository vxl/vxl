include_directories( ${MUL_INCLUDE_DIR} )

add_executable( bvpl_test_all
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

target_link_libraries( bvpl_test_all bvpl bvxm ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib)

add_test( NAME bvpl_test_subgrid COMMAND $<TARGET_FILE:bvpl_test_all> test_subgrid  )
add_test( NAME bvpl_test_neighb_oper COMMAND $<TARGET_FILE:bvpl_test_all> test_neighb_oper  )
add_test( NAME bvpl_test_kernels COMMAND $<TARGET_FILE:bvpl_test_all> test_bvpl_kernels  )
add_test( NAME bvpl_test_bvpl_kernel_functors COMMAND $<TARGET_FILE:bvpl_test_all> test_bvpl_kernel_functors  )
add_test( NAME bvpl_test_direction_to_color_map COMMAND $<TARGET_FILE:bvpl_test_all> test_direction_to_color_map  )
add_test( NAME bvpl_test_vector_operator COMMAND $<TARGET_FILE:bvpl_test_all> test_bvpl_vector_operator  )
add_test( NAME bvpl_test_vector_directions COMMAND $<TARGET_FILE:bvpl_test_all> test_vector_directions  )
if( VXL_RUN_FAILING_TESTS )
add_test( NAME bvpl_test_detect_corner COMMAND $<TARGET_FILE:bvpl_test_all> test_detect_corner  )
endif()

add_executable( bvpl_test_include test_include.cxx )
target_link_libraries( bvpl_test_include bvpl)
add_executable( bvpl_test_template_include test_template_include.cxx )
target_link_libraries( bvpl_test_template_include bvpl)
