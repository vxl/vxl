LINK_LIBRARIES( vil vpl vul vbl testlib )

IF(HAS_IMAGE)
  LINK_DIRECTORIES( Image COOL Numerics GeneralUtility )
  LINK_LIBRARIES( ImageClasses Basics Numerics )
ENDIF(HAS_IMAGE)

IF(HAS_IMAGE)
  ADD_DEFINITIONS(-DHAS_IMAGE=1)
ELSE(HAS_IMAGE)
  ADD_DEFINITIONS(-DHAS_NO_IMAGE=1)
ENDIF(HAS_IMAGE)

ADD_EXECUTABLE( conversions_image_test_all
  test_driver.cxx

  test_vil_vbl_conversions.cxx
  test_image_conversions.cxx
)

ADD_TEST( conversions_test_vil_vbl_conversions conversions_image_test_all test_vil_vbl_conversions )
ADD_TEST( conversions_test_image_conversions conversions_image_test_all test_image_conversions )

ADD_EXECUTABLE( conversions_image_test_include test_include.cxx )
