#
# Find xerces on Windows 
#
# date  : Dec 07, 2002
# author: J. L. Mundy
#

IF(WIN32)

  FIND_PATH(XERCES_BXML_INCLUDE_PATH DOM.hpp
            c:/TargetJr/xerces-c-src1_4_0/include
            /usr/local/xerces-c-src1_4_0/include
  )

  FIND_LIBRARY(XERCES_BXML_LIBRARY_PATH xerces-c_1.lib
               /usr/local/xerces-c-src1_4_0/lib/
  )

  IF(XERCES_BXML_INCLUDE_PATH)
    IF(XERCES_BXML_LIBRARY_PATH)
      IF(NOT HAS_XERCES)
       SET(HAS_XERCES "YES")
       INCLUDE_DIRECTORIES( ${XERCES_BXML_INCLUDE_PATH} )
       LINK_LIBRARIES( ${XERCES_BXML_LIBRARY_PATH} )
      ENDIF(NOT HAS_XERCES)
    ENDIF(XERCES_BXML_LIBRARY_PATH)
  ENDIF(XERCES_BXML_INCLUDE_PATH)

ENDIF(WIN32)
