# mul/vpdfl/tests/CMakeLists.txt

TARGET_LINK_LIBRARIES( test_vpdfl vpdfl testlib )

ADD_EXECUTABLE( test_vpdfl test_vpdfl.cxx )
ADD_TEST(       test_vpdfl test_vpdfl     )
