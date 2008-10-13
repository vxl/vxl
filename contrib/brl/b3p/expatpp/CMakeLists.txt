#This is brl/b3p/expatpp

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF( EXPAT_FOUND )

  IF( VXL_USING_NATIVE_EXPAT )
    INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
  ELSE( VXL_USING_NATIVE_EXPAT )
    INCLUDE_DIRECTORIES( ${BRL_BINARY_DIR}/b3p/expat )
  ENDIF( VXL_USING_NATIVE_EXPAT )

  SET( expatpp_sources expatpp.h expatpp.cpp expatpplib.h )

  ADD_LIBRARY( expatpp ${expatpp_sources} )
  TARGET_LINK_LIBRARIES( expatpp ${EXPAT_LIBRARIES} )

ENDIF( EXPAT_FOUND )
