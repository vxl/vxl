ADD_EXECUTABLE( boxm2_vecf_ocl_test_all
  test_driver.cxx
  test_transform.cxx
  test_store_nbrs.cxx
  test_filter.cxx
 )



TARGET_LINK_LIBRARIES(boxm2_vecf_ocl_test_all testlib  boxm2_vecf boxm2_vecf_ocl brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl )

ADD_TEST( boxm2_vecf_ocl_test_transform   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_vecf_ocl_test_all  test_transform  )
ADD_TEST( boxm2_vecf_ocl_test_store_nbrs  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_vecf_ocl_test_all  test_store_nbrs  )
ADD_TEST( boxm2_vecf_ocl_test_filter      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/boxm2_vecf_ocl_test_all  test_filter  )


#ADD_EXECUTABLE( boxm2_ocl_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_ocl_test_include boxm2_ocl )

