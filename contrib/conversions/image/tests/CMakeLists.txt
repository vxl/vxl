INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vil/CMakeListsLink.txt )
INCLUDE( ${allvxl_SOURCE_DIR}/vxl/vpl/CMakeListsLink.txt )


IF(HAS_IMAGE)
  LINK_DIRECTORIES( Image COOL Numerics GeneralUtility )
  LINK_LIBRARIES( ImageClasses Basics Numerics )
  ADD_EXECUTABLE( test_image_conversions test_image_conversions.cxx )
ENDIF(HAS_IMAGE)
