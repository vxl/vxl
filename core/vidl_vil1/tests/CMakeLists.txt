# This is core/vidl_vil1/tests/CMakeLists.txt

LINK_LIBRARIES( vidl_vil1 )

ADD_EXECUTABLE( vidl_vil1_test_all vidl_vil1_test.cxx )
ADD_TEST( vidl_vil1_test  vidl_vil1_test_all )

ADD_EXECUTABLE( vidl_vil1_test_include test_include.cxx )
