SET( brad_test_sources
  test_driver.cxx
  test_illum.cxx
  test_sun_pos.cxx
  test_sun_hist.cxx
  test_sun_dir_index.cxx
  )

ADD_EXECUTABLE(brad_test_all ${brad_test_sources})
TARGET_LINK_LIBRARIES( brad_test_all brad testlib)

ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_illum)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_pos)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_hist)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_dir_index)


ADD_EXECUTABLE( brad_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brad_test_include brad)

