# mul/mil3d/tests/CMakeLists.txt

TARGET_LINK_LIBRARIES( test_mil3d mil3d testlib )

ADD_EXECUTABLE( test_mil3d test_mil3d.cxx )
ADD_TEST(       test_mil3d test_mil3d     )
