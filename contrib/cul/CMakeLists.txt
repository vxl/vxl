# ./cul/CMakeLists.txt

PROJECT( cul )

doxygen_add_package(contrib/gel "Cornell University Libraries")

SET( VXL_CUL_INCLUDE_DIR ${cul_SOURCE_DIR} CACHE STATIC "Cornell University package include files" )
SET( 
  CUL_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/cul 
  CACHE INTERNAL "Cornell University package include files" )


INCLUDE_DIRECTORIES( ${VXL_CORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${VXL_CUL_INCLUDE_DIR} )

# The bundler library requires BRL, RPL, and GEL
IF(BUILD_BRL AND BUILD_GEL AND BUILD_RPL)
  SUBDIRS( bundler )
ENDIF(BUILD_BRL AND BUILD_GEL AND BUILD_RPL)
