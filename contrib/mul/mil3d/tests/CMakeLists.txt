# ./mul/mil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( test_mil3d test_mil3d.cxx )
TARGET_LINK_LIBRARIES( test_mil3d mil3d )

ADD_TEST(       test_mil3d test_mil3d     )
