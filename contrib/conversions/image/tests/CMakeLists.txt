LINK_DIRECTORIES( conversions vxl )
include ${IUEROOT}/vxl/vil/config.mk
LINK_LIBRARIES( vil vpl )

IF(HAS_IMAGE)
  LINK_DIRECTORIES( Image COOL Numerics GeneralUtility )
  LINK_LIBRARIES( ImageClasses Basics Numerics )
  ADD_EXECUTABLE( test_image_conversions )
ENDIF(HAS_IMAGE)
