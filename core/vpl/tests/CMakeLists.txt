# ./vxl/vpl/tests/CMakeLists.txt

ADD_EXECUTABLE( vpl_test_all vpl_test_all.cxx )
ADD_EXECUTABLE( vpl_test_unistd test_unistd.cxx )

ADD_TEST( vpl_test_all vpl_test_all )
ADD_TEST( vpl_test_unistd vpl_test_unistd )

# Immediate dependencies
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )
