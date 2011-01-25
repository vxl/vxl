# tbl/vepl1/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vepl1_test_all
  test_driver.cxx        test_driver.h

  vepl1_test_threshold.cxx
  vepl1_test_erode_disk.cxx
)
TARGET_LINK_LIBRARIES( vepl1_test_all vepl1 vil1 testlib )

ADD_TEST(vepl1_test_threshold  ${EXECUTABLE_OUTPUT_PATH}/vepl1_test_all vepl1_test_threshold)
ADD_TEST(vepl1_test_erode_disk ${EXECUTABLE_OUTPUT_PATH}/vepl1_test_all vepl1_test_erode_disk)

ADD_EXECUTABLE( vepl1_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vepl1_test_include vepl1 )
ADD_EXECUTABLE( vepl1_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vepl1_test_template_include vepl1 )
