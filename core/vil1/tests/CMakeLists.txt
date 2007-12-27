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

ADD_TEST( vil1_test_flipud            ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_flipud )
ADD_TEST( vil1_test_assign            ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_assign )
ADD_TEST( vil1_test_interpolate       ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_interpolate )
ADD_TEST( vil1_test_load              ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_load )
ADD_TEST( vil1_test_load_gen          ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_load_gen )
ADD_TEST( vil1_test_memory_image      ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_memory_image )
ADD_TEST( vil1_test_memory_image_of   ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_memory_image_of )
#ADD_TEST( vil1_test_save_load_image   ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_save_load_image )
ADD_TEST( vil1_test_write_endian      ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_write_endian )
ADD_TEST( vil1_test_colour_space      ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_colour_space )
ADD_TEST( vil1_test_copy              ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_copy ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
ADD_TEST( vil1_test_file_format_read  ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
ADD_TEST( vil1_test_resample          ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_resample ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
ADD_TEST( vil1_test_vil               ${EXECUTABLE_OUTPUT_PATH}/vil1_test_all test_vil ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

#ADD_EXECUTABLE( vil_vil1_compare_access_timings vil_vil1_compare_access_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_vil1_compare_access_timings vil mbl vcl )
#ADD_TEST( vil_vil1_compare_access_timings ${EXECUTABLE_OUTPUT_PATH}/vil_vil1_compare_access_timings )

#ADD_EXECUTABLE( vil_vil1_compare_function_timings vil_vil1_compare_function_timings.cxx)
#TARGET_LINK_LIBRARIES( vil_vil1_compare_function_timings vil1 mbl vcl )
#ADD_TEST( vil_vil1_compare_function_timings ${EXECUTABLE_OUTPUT_PATH}/vil_vil1_compare_function_timings )

ADD_EXECUTABLE( vil1_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil1_test_include vil1 vil )
