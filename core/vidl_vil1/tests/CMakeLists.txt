# This is core/vidl_vil1/tests/CMakeLists.txt

ADD_EXECUTABLE( vidl_vil1_test vidl_vil1_test.cxx )
TARGET_LINK_LIBRARIES( vidl_vil1_test vidl_vil1 )

ADD_TEST( vidl_vil1_test  vidl_vil1_test )

ADD_EXECUTABLE( vidl_vil1_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vidl_vil1_test_include vidl_vil1 )
