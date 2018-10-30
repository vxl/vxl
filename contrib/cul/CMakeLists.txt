# ./cul/CMakeLists.txt

project( cul )

doxygen_add_package(contrib/gel "Cornell University Libraries")

set( VXL_CUL_INCLUDE_DIR ${cul_SOURCE_DIR} CACHE STATIC "Cornell University package include files" )
set(
  CUL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/cul
  CACHE INTERNAL "Cornell University package include files" )


include_directories( ${VXL_CORE_INCLUDE_DIR} )
include_directories( ${VXL_CUL_INCLUDE_DIR} )

# The bundler library requires BRL, RPL, and GEL
if(VXL_BUILD_BRL AND VXL_BUILD_GEL AND VXL_BUILD_RPL)
  add_subdirectory(bundler)
endif()
