# This is contrib/brl/bbas/vidl2/tests/CMakeLists.txt

ADD_EXECUTABLE( vidl2_test_all
  test_driver.cxx

  test_pixel_format.cxx
)

TARGET_LINK_LIBRARIES( vidl2_test_all vidl2 testlib )

ADD_TEST( vidl2_test_pixel_format             vidl2_test_all  test_pixel_format)



ADD_EXECUTABLE( vidl2_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vidl2_test_include vidl2 )
