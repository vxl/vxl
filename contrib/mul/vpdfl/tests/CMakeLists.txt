# ./mul/vpdfl/tests/CMakeLists.txt

ADD_EXECUTABLE( test_vpdfl test_vpdfl.cxx )
ADD_TEST(       test_vpdfl test_vpdfl     )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/mul/vpdfl/CMakeListsLink.txt )
