# ./mul/mil3d/tests/CMakeLists.txt

ADD_EXECUTABLE( test_mil3d test_mil3d.cxx )
ADD_TEST(       test_mil3d test_mil3d     )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil3d/CMakeListsLink.txt )
