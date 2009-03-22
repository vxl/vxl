# This is core/vidl1/tests/CMakeLists.txt

LINK_LIBRARIES( vidl1 )

ADD_EXECUTABLE( vidl1_test_all vidl1_test.cxx )
ADD_TEST( vidl1_test  ${EXECUTABLE_OUTPUT_PATH}/vidl1_test_all )

ADD_EXECUTABLE( vidl1_test_include test_include.cxx )
