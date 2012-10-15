ADD_EXECUTABLE( boxm2_volm_test_all
  test_driver.cxx
  test_volm_wr3db_ind.cxx
 )

TARGET_LINK_LIBRARIES( boxm2_volm_test_all testlib boxm2_volm )

ADD_TEST( boxm2_volm_test_wr3db_ind_vrml        ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_test_all  test_volm_wr3db_ind  )


ADD_EXECUTABLE( boxm2_volm_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_test_include boxm2_volm)
