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

# this line used for transit phase
FIND_PATH(XERCES_NEW_BXML_INCLUDE_PATH xercesc/dom/Dom.hpp
          ${vxl_SOURCE_DIR}/../xerces-c2_2_0/include
)

FIND_LIBRARY(XERCES_NEW_BXML_LIBRARY_PATH xerces-c_2
          ${vxl_SOURCE_DIR}/../xerces-c2_2_0/lib
          ${vxl_BINARY_DIR}/v3p/lib
)

IF(XERCES_NEW_BXML_INCLUDE_PATH)
  IF(XERCES_NEW_BXML_LIBRARY_PATH)
    IF(NOT HAS_NEW_XERCES)
      SET(HAS_NEW_XERCES "YES")
      INCLUDE_DIRECTORIES( ${XERCES_NEW_BXML_INCLUDE_PATH} )
      SET( NEW_XERCES_LIBRARIES ${XERCES_NEW_BXML_LIBRARY_PATH} )
    ENDIF(NOT HAS_NEW_XERCES)
  ENDIF(XERCES_NEW_BXML_LIBRARY_PATH)
ENDIF(XERCES_NEW_BXML_INCLUDE_PATH)

