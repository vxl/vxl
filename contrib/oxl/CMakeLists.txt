# oxl/CMakeLists.txt

project( oxl )

doxygen_add_package(contrib/oxl "Oxford University Libraries")

set( OXL_INCLUDE_DIR ${oxl_SOURCE_DIR} CACHE STATIC "Oxford University package include files" )
set( OXL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/oxl
  CACHE INTERNAL "Oxford University package include files"
)

include_directories(${OXL_INCLUDE_DIR})

# These four libraries are mutually independent:
add_subdirectory(mvl)
add_subdirectory(vrml)
add_subdirectory(osl)

# using vgui:
if( VXL_BUILD_VGUI )
  add_subdirectory(xcv)
endif()
