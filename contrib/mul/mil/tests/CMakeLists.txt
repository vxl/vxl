# mul/mil/tests/CMakeLists.txt

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_TEST(       test_mil test_mil     )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/algo/CMakeListsLink.txt )
