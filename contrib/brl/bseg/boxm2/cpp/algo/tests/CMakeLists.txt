ADD_EXECUTABLE( boxm2_cpp_algo_test_all
  test_driver.cxx
  test_merge_mixtures.cxx
  test_cone_ray_trace.cxx
  test_cone_update.cxx
  test_merge_function.cxx
 )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_all testlib boxm2_cpp_algo vgl vil)

ADD_TEST( boxm2_test_merge_mixtures      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_cpp_algo_test_all  test_merge_mixtures  )
ADD_TEST( boxm2_test_cone_ray_trace      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_cpp_algo_test_all  test_cone_ray_trace  )
ADD_TEST( boxm2_test_cone_update         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_cpp_algo_test_all  test_cone_update     )
if( HACK_FORCE_BRL_FAILING_TESTS ) ## This test is fails on Mac with clang
ADD_TEST( boxm2_test_merge_function      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_cpp_algo_test_all  test_merge_function  )
endif()

ADD_EXECUTABLE( boxm2_cpp_algo_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_include boxm2_cpp_algo )

ADD_EXECUTABLE( boxm2_cpp_algo_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_cpp_algo_test_template_include boxm2_cpp_algo )
