# ./mul/mil/tests/CMakeLists.txt

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_TEST(       test_mil test_mil     )
ADD_EXECUTABLE( mil_test_sample_grid_2d test_sample_grid_2d.cxx )
ADD_TEST( mil_test_sample_grid_2d mil_test_sample_grid_2d )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/algo/CMakeListsLink.txt )
