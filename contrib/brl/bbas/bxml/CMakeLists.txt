# vxl/contrib/brl/bbas/bxml/CMakeLists.txt
# The Development Brown XML Parser

doxygen_add_library(contrib/brl/bbas/bxml
  DEPENDS contrib/brl/bbas/bdgl core/vbl_io
  PACKAGE contrib/brl
  )

SET( bxml_sources
    bxml_document.cxx            bxml_document.h
    bxml_write.cxx               bxml_write.h
    bxml_find.cxx                bxml_find.h
)

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )
  SET( bxml_sources ${bxml_sources}
      bxml_read.cxx                bxml_read.h
  )
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

AUX_SOURCE_DIRECTORY(Templates bxml_sources)

ADD_LIBRARY(bxml ${bxml_sources})

TARGET_LINK_LIBRARIES( bxml vbl_io vul vbl )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  TARGET_LINK_LIBRARIES( bxml ${EXPATPP_LIBRARIES})
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

SUBDIRS( bsvg )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

#install the .h .txx and libs
INSTALL_TARGETS(/lib bxml)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bxml/ ${bxml_sources})
