# mul/vpdfl/tests/CMakeLists.txt

LINK_LIBRARIES( vpdfl vpl testlib )

ADD_EXECUTABLE( test_vpdfl test_vpdfl.cxx )
ADD_TEST(       test_vpdfl test_vpdfl     )
