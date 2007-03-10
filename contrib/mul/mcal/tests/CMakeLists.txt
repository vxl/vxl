# mul/mcal/tests/CMakeLists.txt

ADD_EXECUTABLE( mcal_test_all
  test_driver.cxx
  test_pca.cxx
  )
TARGET_LINK_LIBRARIES( mcal_test_all mcal testlib )

ADD_TEST( mcal_test_pca ${EXECUTABLE_OUTPUT_PATH}/mcal_test_all test_pca)

ADD_EXECUTABLE( mcal_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mcal_test_include mcal )
