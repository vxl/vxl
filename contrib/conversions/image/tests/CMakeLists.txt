LINK_LIBRARIES( vil1 vpl vul vbl testlib )

SET(HAS_IMAGE "NO")
IF(EXISTS ${vxl_SOURCE_DIR}/Image/CMakeLists.txt )
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Image)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/COOL)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/Numerics)
  INCLUDE_DIRECTORIES(${vxl_SOURCE_DIR}/GeneralUtility)
  LINK_LIBRARIES( ImageClasses Basics Numerics )
  SET(HAS_IMAGE "YES")
ENDIF(EXISTS ${vxl_SOURCE_DIR}/Image/CMakeLists.txt )

IF(HAS_IMAGE)
  ADD_DEFINITIONS(-DHAS_IMAGE=1)
ELSE(HAS_IMAGE)
  ADD_DEFINITIONS(-DHAS_NO_IMAGE=1)
ENDIF(HAS_IMAGE)

ADD_EXECUTABLE( conversions_image_test_all
  test_driver.cxx

  test_vil1_vbl_conversions.cxx
  test_image_conversions.cxx
)

ADD_TEST( conversions_test_vil1_vbl_conversions conversions_image_test_all test_vil1_vbl_conversions )
ADD_TEST( conversions_test_image_conversions conversions_image_test_all test_image_conversions )

ADD_EXECUTABLE( conversions_image_test_include test_include.cxx )
