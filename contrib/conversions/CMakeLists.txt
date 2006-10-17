PROJECT( conversions )

SET( CONVERSIONS_INCLUDE_DIR ${conversions_SOURCE_DIR} CACHE STATIC "Conversions Examples package include files" )
SET( CONVERSIONS_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/conversions CACHE INTERNAL "Conversions Examples package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${CONVERSIONS_INCLUDE_DIR} )

SUBDIRS(container geometry image math misc)
