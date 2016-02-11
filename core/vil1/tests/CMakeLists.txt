# This is core/vil1/tests/CMakeLists.txt

add_executable( vil1_test_all
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
target_link_libraries( vil1_test_all ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vcl )

add_test( NAME vil1_test_flipud COMMAND $<TARGET_FILE:vil1_test_all> test_flipud )
add_test( NAME vil1_test_assign COMMAND $<TARGET_FILE:vil1_test_all> test_assign )
add_test( NAME vil1_test_interpolate COMMAND $<TARGET_FILE:vil1_test_all> test_interpolate )
add_test( NAME vil1_test_load COMMAND $<TARGET_FILE:vil1_test_all> test_load )
add_test( NAME vil1_test_load_gen COMMAND $<TARGET_FILE:vil1_test_all> test_load_gen )
add_test( NAME vil1_test_memory_image COMMAND $<TARGET_FILE:vil1_test_all> test_memory_image )
add_test( NAME vil1_test_memory_image_of COMMAND $<TARGET_FILE:vil1_test_all> test_memory_image_of )
#add_test( NAME vil1_test_save_load_image COMMAND $<TARGET_FILE:vil1_test_all> test_save_load_image )
add_test( NAME vil1_test_write_endian COMMAND $<TARGET_FILE:vil1_test_all> test_write_endian )
add_test( NAME vil1_test_colour_space COMMAND $<TARGET_FILE:vil1_test_all> test_colour_space )
add_test( NAME vil1_test_copy COMMAND $<TARGET_FILE:vil1_test_all> test_copy ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
add_test( NAME vil1_test_file_format_read COMMAND $<TARGET_FILE:vil1_test_all> test_file_format_read ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data )
add_test( NAME vil1_test_resample COMMAND $<TARGET_FILE:vil1_test_all> test_resample ${CMAKE_CURRENT_SOURCE_DIR}/square.pgm )
add_test( NAME vil1_test_vil COMMAND $<TARGET_FILE:vil1_test_all> test_vil ${CMAKE_CURRENT_SOURCE_DIR}/file_read_data)

#add_executable( vil_vil1_compare_access_timings vil_vil1_compare_access_timings.cxx)
#target_link_libraries( vil_vil1_compare_access_timings ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_vil1_compare_access_timings COMMAND $<TARGET_FILE:vil_vil1_compare_access_timings> )

#add_executable( vil_vil1_compare_function_timings vil_vil1_compare_function_timings.cxx)
#target_link_libraries( vil_vil1_compare_function_timings ${VXL_LIB_PREFIX}vil1 mbl ${VXL_LIB_PREFIX}vcl )
#add_test( NAME vil_vil1_compare_function_timings COMMAND $<TARGET_FILE:vil_vil1_compare_function_timings> )

add_executable( vil1_test_include test_include.cxx )
target_link_libraries( vil1_test_include ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil )
add_executable( vil1_test_template_include test_template_include.cxx )
target_link_libraries( vil1_test_template_include ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil )
