# This is core/vidl/tests/CMakeLists.txt

LINK_LIBRARIES( vidl )

ADD_EXECUTABLE( vidl_test_all vidl_test.cxx )
ADD_TEST( vidl_test  vidl_test_all )

ADD_EXECUTABLE( vidl_test_include test_include.cxx )
