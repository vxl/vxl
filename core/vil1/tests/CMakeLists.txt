#
# ./vxl/vil/tests/CMakeLists.txt
#
#   geoff.cross@crd.ge.com
#

INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )

ADD_EXECUTABLE( test_all             test_all.cxx )
ADD_EXECUTABLE( test_assign          test_assign.cxx )
ADD_EXECUTABLE( test_interpolate     test_interpolate.cxx )
ADD_EXECUTABLE( test_load            test_load.cxx )
ADD_EXECUTABLE( test_memory_image    test_memory_image.cxx )
ADD_EXECUTABLE( test_memort_image_of test_memory_image_of.cxx )
ADD_EXECUTABLE( test_save_load_image test_save_load_image.cxx )
ADD_EXECUTABLE( test_write_endian    test_write_endian.cxx )
