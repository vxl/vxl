#
# ./vxl/vil/tests/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
# test_interpolate uses pow():
IF(UNIX)
  LINK_LIBRARIES( m )
ENDIF(UNIX)

ADD_EXECUTABLE( vil_test_all         vil_test_all.cxx )
ADD_EXECUTABLE( test_flipud          test_flipud.cxx )
ADD_EXECUTABLE( test_assign          test_assign.cxx )
ADD_EXECUTABLE( test_interpolate     test_interpolate.cxx )
ADD_EXECUTABLE( test_load            test_load.cxx )
ADD_EXECUTABLE( test_memory_image    test_memory_image.cxx )
ADD_EXECUTABLE( test_memory_image_of test_memory_image_of.cxx )
ADD_EXECUTABLE( test_save_load_image test_save_load_image.cxx )
ADD_EXECUTABLE( test_write_endian    test_write_endian.cxx )
ADD_EXECUTABLE( test_colour_space    test_colour_space.cxx )


ADD_TEST( vil_test_all         	   vil_test_all )
ADD_TEST( vil_test_flipud          test_flipud )
ADD_TEST( vil_test_assign          test_assign )
ADD_TEST( vil_test_interpolate     test_interpolate )
ADD_TEST( vil_test_load            test_load )
ADD_TEST( vil_test_memory_image    test_memory_image )
ADD_TEST( vil_test_memory_image_of test_memory_image_of )
ADD_TEST( vil_test_save_load_image test_save_load_image )
ADD_TEST( vil_test_write_endian    test_write_endian )
ADD_TEST( vil_test_colour_space    test_colour_space )

