INCLUDE( ${allvxl_SOURCE_DIR}/conversions/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vbl/CMakeListsLink.txt )

ADD_EXECUTABLE( test_vil_vbl_conversions test_vil_vbl_conversions )
ADD_TEST( conversions_test_vil_vbl_conversions test_vil_vbl_conversions )

IF(HAS_IMAGE)
  LINK_DIRECTORIES( Image COOL Numerics GeneralUtility )
  LINK_LIBRARIES( ImageClasses Basics Numerics )
  ADD_EXECUTABLE( test_image_conversions test_image_conversions )
  ADD_TEST( conversions_test_image_conversions test_image_conversions )
ENDIF(HAS_IMAGE)
