PROJECT( conversions )

SET( CONVERSIONS_INCLUDE_DIR ${conversions_SOURCE_DIR} CACHE STATIC "Conversions Examples package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CONVERSIONS_INCLUDE_DIR} )

SUBDIRS(container geometry image math misc)
