# ./mul/mil/tests/CMakeLists.txt

LINK_LIBRARIES( mil_algo mil )

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_TEST(       test_mil test_mil     )
ADD_EXECUTABLE( mil_test_gauss_reduce_2d test_gauss_reduce_2d.cxx )
ADD_TEST( mil_test_gauss_reduce_2d mil_test_gauss_reduce_2d )
