INCLUDE( ${allvxl_SOURCE_DIR}/conversions/image/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/testlib/CMakeListsLink.txt )

ADD_EXECUTABLE(  conversions_image_test_driver
  test_driver.cxx

  test_vil_vbl_conversions.cxx
IF(HAS_IMAGE)
  test_image_conversions.cxx
ENDIF(HAS_IMAGE)
)
ADD_TEST( conversions_test_vil_vbl_conversions conversions_image_test_driver test_vil_vbl_conversions )

IF(HAS_IMAGE)
  ADD_TEST( conversions_test_image_conversions conversions_image_test_driver test_image_conversions )
ENDIF(HAS_IMAGE)
