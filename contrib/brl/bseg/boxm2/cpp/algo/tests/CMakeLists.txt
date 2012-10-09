ADD_EXECUTABLE( boxm2_cpp_algo_test_all
  test_driver.cxx
  test_merge_mixtures.cxx
  test_cone_ray_trace.cxx
  test_cone_update.cxx
  test_merge_function.cxx
 )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_all testlib boxm2_cpp_algo vgl vil)

ADD_TEST( boxm2_test_merge_mixtures      ${EXECUTABLE_OUTPUT_PATH}/boxm2_cpp_algo_test_all  test_merge_mixtures  )
ADD_TEST( boxm2_test_cone_ray_trace      ${EXECUTABLE_OUTPUT_PATH}/boxm2_cpp_algo_test_all  test_cone_ray_trace  )
ADD_TEST( boxm2_test_cone_update         ${EXECUTABLE_OUTPUT_PATH}/boxm2_cpp_algo_test_all  test_cone_update     )
ADD_TEST( boxm2_test_merge_function      ${EXECUTABLE_OUTPUT_PATH}/boxm2_cpp_algo_test_all  test_merge_function  )

ADD_EXECUTABLE( boxm2_cpp_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_include boxm2_cpp_algo )

ADD_EXECUTABLE( boxm2_cpp_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_template_include boxm2_cpp_algo )
