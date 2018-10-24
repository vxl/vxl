project( rpl )

doxygen_add_package(contrib/rpl "RPI Libraries")

set( RPL_INCLUDE_DIR ${rpl_SOURCE_DIR} CACHE STATIC "RPI package include files" )
set( RPL_BINARY_INCLUDE_DIR ${PROJECT_BINARY_DIR} )
set( RPL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/rpl
  CACHE INTERNAL "RPI package include files"
)

include_directories( ${RPL_INCLUDE_DIR} )

add_subdirectory(rrel)
add_subdirectory(rsdl)
if(VXL_BUILD_MUL)
  add_subdirectory(rgrl)
endif()

if(VXL_BUILD_RPL_RGTL)
  add_subdirectory(rgtl)
  if(VXL_BUILD_RPL_RTVL)
    add_subdirectory(rtvl)
  endif()
endif()
