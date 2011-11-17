# vxl/vpgl/xio/CMakeLists.txt

SET(vpgl_xio_sources
  vpgl_xio_lvcs.cxx   vpgl_xio_lvcs.h
)

ADD_LIBRARY(vpgl_xio ${vpgl_xio_sources} )
TARGET_LINK_LIBRARIES( vpgl_xio vpgl vsl )

INSTALL_TARGETS(/lib vpgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpgl/xio ${vpgl_xio_sources})

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
