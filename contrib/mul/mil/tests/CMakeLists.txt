#
# ./mul/mil/tests/CMakeLists.txt
#
#   Ian Scott
#

INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/algo/CMakeListsLink.txt )

ADD_EXECUTABLE( test_mil test_mil.cxx )
ADD_EXECUTABLE( test_gauss_reduce_2d test_gauss_reduce_2d.cxx )
