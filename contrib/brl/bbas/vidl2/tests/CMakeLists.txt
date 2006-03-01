# This is contrib/brl/bbas/vidl2/tests/CMakeLists.txt

ADD_EXECUTABLE( vidl2_test_all
  test_driver.cxx

  test_pixel_format.cxx
  test_pixel_iterator.cxx
  test_color.cxx
  test_convert.cxx
)
TARGET_LINK_LIBRARIES( vidl2_test_all vidl2 vul testlib )

ADD_TEST( vidl2_test_pixel_format             vidl2_test_all  test_pixel_format )
ADD_TEST( vidl2_test_pixel_iterator           vidl2_test_all  test_pixel_iterator )
ADD_TEST( vidl2_test_color                    vidl2_test_all  test_color )
ADD_TEST( vidl2_test_convert                  vidl2_test_all  test_convert )

ADD_EXECUTABLE( vidl2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vidl2_test_include vidl2 )
