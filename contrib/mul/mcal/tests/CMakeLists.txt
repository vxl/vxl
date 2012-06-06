# mul/mcal/tests/CMakeLists.txt

ADD_EXECUTABLE( mcal_test_all
  test_driver.cxx
  test_pca.cxx
  test_trivial_ca.cxx
  test_general_ca.cxx
  )
TARGET_LINK_LIBRARIES( mcal_test_all mcal testlib )

ADD_TEST( mcal_test_pca ${EXECUTABLE_OUTPUT_PATH}/mcal_test_all test_pca)
ADD_TEST( mcal_test_trivial_ca ${EXECUTABLE_OUTPUT_PATH}/mcal_test_all test_trivial_ca)
ADD_TEST( mcal_test_general_ca ${EXECUTABLE_OUTPUT_PATH}/mcal_test_all test_general_ca)

ADD_EXECUTABLE( mcal_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( mcal_test_include mcal )
