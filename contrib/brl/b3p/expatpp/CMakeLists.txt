#This is brl/b3p/expatpp


INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF (EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expat )
SET(expatpp_sources expatpp.h expatpp.cpp expatpplib.h )

ADD_LIBRARY(expatpp ${expatpp_sources})
TARGET_LINK_LIBRARIES(expatpp ${EXPAT_LIBRARIES})


ENDIF (EXPAT_FOUND)
