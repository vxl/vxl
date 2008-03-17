# vxl/contrib/brl/bbas/bxml/CMakeLists.txt
# The Development Brown XML Parser

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )
INCLUDE_DIRECTORIES ( ${EXPAT_INCLUDE_DIR} )

SET( bxml_sources
    bxml_document.cxx            bxml_document.h
    bxml_read.cxx                bxml_read.h
    bxml_write.cxx               bxml_write.h
    bxml_find.cxx                bxml_find.h
)

AUX_SOURCE_DIRECTORY(Templates bxml_sources)

ADD_LIBRARY(bxml ${bxml_sources})

TARGET_LINK_LIBRARIES( bxml expatpp vbl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
