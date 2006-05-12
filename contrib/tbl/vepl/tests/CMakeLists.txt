# ./tbl/vepl/tests/CMakeLists.txt

ADD_DEFINITIONS(-DINSTANTIATE_TEMPLATES)

ADD_EXECUTABLE( vepl_test_all
  test_driver.cxx        test_driver.h

  vepl_test_threshold.cxx
  vepl_test_erode_disk.cxx
)
TARGET_LINK_LIBRARIES( vepl_test_all vepl vil1 testlib )

ADD_TEST(vepl_test_threshold  ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_threshold)
ADD_TEST(vepl_test_erode_disk ${EXECUTABLE_OUTPUT_PATH}/vepl_test_all vepl_test_erode_disk)

ADD_EXECUTABLE( vepl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vepl_test_include vepl )
