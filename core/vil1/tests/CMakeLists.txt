# This is core/vil1/tests/CMakeLists.txt

ADD_EXECUTABLE( vil1_test_all
  test_driver.cxx

  test_vil.cxx
  test_flipud.cxx
  test_assign.cxx
  test_interpolate.cxx
  test_load.cxx
  test_load_gen.cxx
  test_memory_image.cxx
  test_memory_image_of.cxx
  test_save_load_image.cxx
  test_write_endian.cxx
  test_colour_space.cxx
  test_copy.cxx
  test_file_format_read.cxx
  test_resample.cxx
)
TARGET_LINK_LIBRARIES( vil1_test_all vil1 vil vpl vul testlib vcl )

ADD_TEST( vil1_test_flipud            vil1_test_all test_flipud )
ADD_TEST( vil1_test_assign            vil1_test_all test_assign )
ADD_TEST( vil1_test_interpolate       vil1_test_all test_interpolate )
ADD_TEST( vil1_test_load              vil1_test_all test_load )
ADD_TEST( vil1_test_load_gen          vil1_test_all test_load_gen )
ADD_TEST( vil1_test_memory_image      vil1_test_all test_memory_image )
ADD_TEST( vil1_test_memory_image_of   vil1_test_all test_memory_image_of )
ADD_TEST( vil1_test_save_load_image   vil1_test_all test_save_load_image )
ADD_TEST( vil1_test_write_endian      vil1_test_all test_write_endian )
ADD_TEST( vil1_test_colour_space      vil1_test_all test_colour_space )
ADD_TEST( vil1_test_copy              vil1_test_all test_copy ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
ADD_TEST( vil1_test_file_format_read  vil1_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
ADD_TEST( vil1_test_resample          vil1_test_all test_resample ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
ADD_TEST( vil1_test_vil               vil1_test_all test_vil ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

ADD_EXECUTABLE( vil1_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil1_test_include vil1 )
