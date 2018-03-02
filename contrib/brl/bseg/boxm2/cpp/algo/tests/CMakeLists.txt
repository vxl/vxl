add_executable( boxm2_cpp_algo_test_all
  test_driver.cxx
  test_merge_mixtures.cxx
  test_cone_ray_trace.cxx
  test_cone_update.cxx
  test_merge_function.cxx
 )
target_link_libraries( boxm2_cpp_algo_test_all ${VXL_LIB_PREFIX}testlib boxm2_cpp_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil)

add_test( NAME boxm2_test_merge_mixtures COMMAND $<TARGET_FILE:boxm2_cpp_algo_test_all>  test_merge_mixtures  )
add_test( NAME boxm2_test_cone_ray_trace COMMAND $<TARGET_FILE:boxm2_cpp_algo_test_all>  test_cone_ray_trace  )
add_test( NAME boxm2_test_cone_update COMMAND $<TARGET_FILE:boxm2_cpp_algo_test_all>  test_cone_update     )
if( VXL_RUN_FAILING_TESTS ) ## This test is fails on Mac with clang
add_test( NAME boxm2_test_merge_function COMMAND $<TARGET_FILE:boxm2_cpp_algo_test_all>  test_merge_function  )
endif()

add_executable( boxm2_cpp_algo_test_include test_include.cxx )
target_link_libraries( boxm2_cpp_algo_test_include boxm2_cpp_algo )

add_executable( boxm2_cpp_algo_test_template_include test_template_include.cxx )
target_link_libraries( boxm2_cpp_algo_test_template_include boxm2_cpp_algo )
