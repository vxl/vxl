# ./mul/CMakeLists.txt

PROJECT( mul )

SET( MUL_INCLUDE_DIR ${mul_SOURCE_DIR} CACHE STATIC "Manchester University package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${MUL_INCLUDE_DIR} )

SUBDIRS(mbl mil mil3d vpdfl pdf1d clsfy vil2 vimt vil3d)
