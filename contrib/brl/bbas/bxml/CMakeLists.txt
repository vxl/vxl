# vxl/contrib/brl/bbas/bxml/CMakeLists.txt
# The Development Brown XML Parser

IF(BUILD_EXPAT)
INCLUDE_DIRECTORIES (${BRL_INCLUDE_DIR}/b3p/expatpp )

SET( bxml_sources
    bxml_document.cxx            bxml_document.h
    bxml_read.cxx                bxml_read.h
    bxml_write.cxx               bxml_write.h
    bxml_find.cxx                bxml_find.h
)


ADD_DEFINITIONS(-DHAS_BXML)

AUX_SOURCE_DIRECTORY(Templates bxml_sources)

ADD_LIBRARY(bxml ${bxml_sources})

TARGET_LINK_LIBRARIES( bxml expatpp vbl )
IF ( APPLE )
  TARGET_LINK_LIBRARIES(bxml expat)
ENDIF ( APPLE )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(BUILD_EXPAT)
