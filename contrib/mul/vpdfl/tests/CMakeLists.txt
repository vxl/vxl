# ./mul/vpdfl/tests/CMakeLists.txt

ADD_EXECUTABLE( test_vpdfl test_vpdfl.cxx )
TARGET_LINK_LIBRARIES( test_vpdfl vpdfl )

ADD_TEST(       test_vpdfl test_vpdfl     )
