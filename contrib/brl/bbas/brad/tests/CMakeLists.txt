SET( brad_test_sources
  test_driver.cxx
  test_illum.cxx
  )

ADD_EXECUTABLE(brad_test_all ${brad_test_sources})
TARGET_LINK_LIBRARIES( brad_test_all brad testlib)

ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_illum)


ADD_EXECUTABLE( brad_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brad_test_include brad)

