# basic/bxml/tests/CMakeLists.txt
# Tests for bxml library

ADD_EXECUTABLE( bxml_test_all
  test_driver.cxx
  test_io.cxx
  test_find.cxx
)

TARGET_LINK_LIBRARIES( bxml_test_all bxml testlib )

ADD_TEST( bxml_test_io       ${EXECUTABLE_OUTPUT_PATH}/bxml_test_all
                                 test_io ${CMAKE_CURRENT_SOURCE_DIR}/ )
ADD_TEST( bxml_test_find     ${EXECUTABLE_OUTPUT_PATH}/bxml_test_all
                                 test_find ${CMAKE_CURRENT_SOURCE_DIR}/ )

ADD_EXECUTABLE( bxml_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bxml_test_include bxml )
