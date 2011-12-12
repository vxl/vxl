# oxl/CMakeLists.txt

PROJECT( oxl )

doxygen_add_package(contrib/oxl "Oxford University Libraries")

SET( OXL_INCLUDE_DIR ${oxl_SOURCE_DIR} CACHE STATIC "Oxford University package include files" )
SET( OXL_INSTALL_INCLUDE_DIR
  ${CMAKE_INSTALL_PREFIX}/include/vxl/contrib/oxl
  CACHE INTERNAL "Oxford University package include files"
)

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

# These four libraries are mutually independent:
SUBDIRS(mvl vrml osl)

# using vgui:
IF( BUILD_VGUI )
  SUBDIRS(xcv apps)
ENDIF( BUILD_VGUI )
