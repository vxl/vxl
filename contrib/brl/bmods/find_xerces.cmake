#
# xerces 1.4
#
# date  : Dec 07, 2002
# author: J. L. Mundy
#

FIND_PATH(XERCES_BXML_INCLUDE_PATH dom/DOM.hpp
          c:/TargetJr/xerces-c-src1_4_0/include
          /usr/local/xerces-c-src1_4_0/include
          ${vxl_SOURCE_DIR}/v3p/xerces-c-src1_4_0/include
)

FIND_LIBRARY(XERCES_BXML_LIBRARY_PATH xerces-c
             /usr/local/xerces-c-src1_4_0/lib
             ${vxl_BINARY_DIR}/lib
             ${vxl_BINARY_DIR}/v3p/lib
             ${vxl_BINARY_DIR}/v3p/xerces-c-src1_4_0
)

IF(XERCES_BXML_INCLUDE_PATH)
  IF(XERCES_BXML_LIBRARY_PATH)
    IF(NOT HAS_XERCES)
      SET(HAS_XERCES "YES")
      INCLUDE_DIRECTORIES( ${XERCES_BXML_INCLUDE_PATH} )
      SET( XERCES_LIBRARIES ${XERCES_BXML_LIBRARY_PATH} )
    ENDIF(NOT HAS_XERCES)
  ENDIF(XERCES_BXML_LIBRARY_PATH)
ENDIF(XERCES_BXML_INCLUDE_PATH)
