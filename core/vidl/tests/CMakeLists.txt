# This is core/vidl/tests/CMakeLists.txt

add_executable( vidl_test_all
  test_driver.cxx

  test_pixel_format.cxx
  test_pixel_iterator.cxx
  test_color.cxx
  test_convert.cxx
)
target_link_libraries( vidl_test_all ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )

add_test( NAME vidl_test_pixel_format COMMAND $<TARGET_FILE:vidl_test_all>  test_pixel_format )
add_test( NAME vidl_test_pixel_iterator COMMAND $<TARGET_FILE:vidl_test_all>  test_pixel_iterator )
add_test( NAME vidl_test_color COMMAND $<TARGET_FILE:vidl_test_all>  test_color )
add_test( NAME vidl_test_convert COMMAND $<TARGET_FILE:vidl_test_all>  test_convert )

add_executable( vidl_test_include test_include.cxx )
target_link_libraries( vidl_test_include ${VXL_LIB_PREFIX}vidl )
add_executable( vidl_test_template_include test_template_include.cxx )
target_link_libraries( vidl_test_template_include ${VXL_LIB_PREFIX}vidl )
