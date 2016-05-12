project( conversions )

set( CONVERSIONS_INCLUDE_DIR ${conversions_SOURCE_DIR} CACHE STATIC "Conversions Examples package include files" )
set( CONVERSIONS_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/conversions CACHE INTERNAL "Conversions Examples package include files" )

include_directories( ${CONVERSIONS_INCLUDE_DIR} )

add_subdirectory(container)
add_subdirectory(geometry)
add_subdirectory(image)
add_subdirectory(math)
add_subdirectory(misc)
