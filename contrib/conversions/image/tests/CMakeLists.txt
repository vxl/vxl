INCLUDE( ${allvxl_SOURCE_DIR}/conversions/image/CMakeListsLink.txt )

ADD_EXECUTABLE( test_vil_vbl_conversions test_vil_vbl_conversions.cxx )
ADD_TEST( conversions_test_vil_vbl_conversions test_vil_vbl_conversions )

IF(HAS_IMAGE)
  ADD_EXECUTABLE( test_image_conversions test_image_conversions.cxx )
  ADD_TEST( conversions_test_image_conversions test_image_conversions )
ENDIF(HAS_IMAGE)
