INCLUDE( ${allvxl_SOURCE_DIR}/conversions/image/CMakeListsLink.txt )
LINK_LIBRARIES( testlib )

IF(HAS_IMAGE)
ELSE(HAS_IMAGE)
  ADD_DEFINITIONS(-DHAS_NO_IMAGE=1)
ENDIF(HAS_IMAGE)

ADD_EXECUTABLE(  conversions_image_test_driver
  test_driver.cxx

  test_vil_vbl_conversions.cxx
  test_image_conversions.cxx
)

ADD_TEST( conversions_test_vil_vbl_conversions conversions_image_test_driver test_vil_vbl_conversions )
ADD_TEST( conversions_test_image_conversions conversions_image_test_driver test_image_conversions )
