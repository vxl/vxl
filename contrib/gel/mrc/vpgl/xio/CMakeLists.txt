# vxl/gel/mrc/vpgl/xio/CMakeLists.txt

SET(vpgl_xio_sources
  vpgl_xio_calibration_matrix.txx vpgl_xio_calibration_matrix.h
)

AUX_SOURCE_DIRECTORY(Templates vpgl_xio_sources)

ADD_LIBRARY(vpgl_xio ${vpgl_xio_sources})
TARGET_LINK_LIBRARIES( vpgl_xio vpgl vsl vgl_xio)

INSTALL_TARGETS(/lib vpgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/gel/mrc/vpgl/xio ${vpgl_xio_sources})

#IF( BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING)
