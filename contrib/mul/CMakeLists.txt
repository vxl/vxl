# ./mul/CMakeLists.txt

PROJECT( mul )

SET( VXL_MUL_INCLUDE_DIR ${mul_SOURCE_DIR} CACHE STATIC "Manchester University package include files" )
SET( MUL_INCLUDE_DIR ${VXL_MUL_INCLUDE_DIR} CACHE STATIC "Deprecated variable giving Manchester University package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SUBDIRS(mbl vpdfl pdf1d clsfy vimt ipts vil3d vimt3d mvl2)
IF (BUILD_UNMAINTAINED_LIBRARIES)
  SUBDIRS(mil mil3d)
ENDIF (BUILD_UNMAINTAINED_LIBRARIES) 