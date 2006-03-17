# oxl/CMakeLists.txt

PROJECT( oxl )

SET( OXL_INCLUDE_DIR ${oxl_SOURCE_DIR} CACHE STATIC "Oxford University package include files" )
SET( OXL_INSTALL_INCLUDE_DIR 
  ${CMAKE_INSTALL_PREFIX}/include/contrib/oxl
  CACHE INTERNAL "Oxford University package include files"
)

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OXL_INCLUDE_DIR})

# These four libraries are mutually independent:
SUBDIRS(mvl oxp vrml osl)

# using vgui:
SUBDIRS(xcv VPLAYER apps)
