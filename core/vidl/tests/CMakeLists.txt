# This is core/vidl/tests/CMakeLists.txt

ADD_EXECUTABLE( vidl_test vidl_test.cxx )
TARGET_LINK_LIBRARIES( vidl_test vidl )

ADD_TEST( vidl_test  vidl_test )

ADD_EXECUTABLE( vidl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vidl_test_include vidl )
