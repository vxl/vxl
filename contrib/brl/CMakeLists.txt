# ./brl/CMakeLists.txt

PROJECT( brl )

SET( BRL_INCLUDE_DIR ${brl_SOURCE_DIR} CACHE STATIC "Brown University package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

# video view library
SUBDIRS(vvid)
