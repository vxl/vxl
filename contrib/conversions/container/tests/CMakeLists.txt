LINK_DIRECTORIES( conversions vxl )
LINK_LIBRARIES( vnl ) # for vnl_test.h

ADD_EXECUTABLE( test_container )

IF(HAS_COOL)
  LINK_DIRECTORIES( COOL Numerics )
  LINK_LIBRARIES( TJCOOL iue-math )
  ADD_EXECUTABLE( test_COOL_container )
ENDIF(HAS_COOL)
