ADD_EXECUTABLE( boxm2_vecf_test_all
  test_driver.cxx
  test_eye.cxx
 )



TARGET_LINK_LIBRARIES(boxm2_vecf_test_all testlib  boxm2_vecf brdb vpgl_algo vpgl_pro vil_pro sdet vnl vgl vil vul vpl )

ADD_TEST( boxm2_vecf_test_eye      ${EXECUTABLE_OUTPUT_PATH}/boxm2_vecf_test_all  test_eye  )

#ADD_EXECUTABLE( boxm2_vecf_test_include test_include.cxx )
#TARGET_LINK_LIBRARIES( boxm2_vecf_test_include boxm2_vecf )

