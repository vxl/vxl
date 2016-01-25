# This is core/vidl/tests/CMakeLists.txt

ADD_EXECUTABLE( vidl_test_all
  test_driver.cxx

  test_pixel_format.cxx
  test_pixel_iterator.cxx
  test_color.cxx
  test_convert.cxx
)
TARGET_LINK_LIBRARIES( vidl_test_all vidl vil vul testlib )

ADD_TEST( NAME vidl_test_pixel_format COMMAND $<TARGET_FILE:vidl_test_all>  test_pixel_format )
ADD_TEST( NAME vidl_test_pixel_iterator COMMAND $<TARGET_FILE:vidl_test_all>  test_pixel_iterator )
ADD_TEST( NAME vidl_test_color COMMAND $<TARGET_FILE:vidl_test_all>  test_color )
ADD_TEST( NAME vidl_test_convert COMMAND $<TARGET_FILE:vidl_test_all>  test_convert )

ADD_EXECUTABLE( vidl_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vidl_test_include vidl )
ADD_EXECUTABLE( vidl_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( vidl_test_template_include vidl )
