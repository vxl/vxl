LINK_DIRECTORIES( conversions vxl )
LINK_LIBRARIES( vnl vbl )

ADD_EXECUTABLE( test_matrix_conversions )

IF(HAS_COOL)
  LINK_DIRECTORIES( COOL Numerics )
  LINK_LIBRARIES( TJCOOL )
  ADD_EXECUTABLE( test_CoolMatrix_conversions )
ENDIF(HAS_COOL)

IF(HAS_NUMERICS)
  LINK_DIRECTORIES( Numerics )
  LINK_LIBRARIES( Numerics )
  ADD_EXECUTABLE( test_IUE_matrix_conversions )
ENDIF(HAS_NUMERICS)
