#
# ./vxl/vil/tests/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )

ADD_EXECUTABLE( test_assign test_assign )
ADD_EXECUTABLE( test_interpolate test_interpolate )
ADD_EXECUTABLE( test_load test_load )
ADD_EXECUTABLE( test_memory_image test_memory_image )
ADD_EXECUTABLE( test_memort_image_of test_memory_image_of )
ADD_EXECUTABLE( test_save_load_image test_save_load_image )
ADD_EXECUTABLE( test_write_endian test_write_endian )
