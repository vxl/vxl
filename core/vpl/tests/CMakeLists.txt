INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vcl/CMakeListsLink.txt )

ADD_EXECUTABLE( test_unistd test_unistd.cxx )

ADD_TEST( vpl_test test_unistd )
