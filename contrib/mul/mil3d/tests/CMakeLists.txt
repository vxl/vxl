# mul/mil3d/tests/CMakeLists.txt

LINK_LIBRARIES( mil3d vpl testlib )
ADD_EXECUTABLE(mil3d_test_all test_mil3d.cxx)
ADD_TEST(test_mil3d mil3d_test_all)

ADD_EXECUTABLE(mil3d_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(mil3d_test_include mil3d)
