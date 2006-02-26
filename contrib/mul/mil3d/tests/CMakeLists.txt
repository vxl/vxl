# mul/mil3d/tests/CMakeLists.txt

ADD_EXECUTABLE(mil3d_test_all test_mil3d.cxx)
TARGET_LINK_LIBRARIES(mil3d_test_all mil3d mil vgl vnl vsl vpl testlib )
ADD_TEST(mil3d_test_all mil3d_test_all)

ADD_EXECUTABLE(mil3d_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(mil3d_test_include mil3d)
