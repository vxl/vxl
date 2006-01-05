# vxl/vgl/xio/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET(vgl_xio_sources
  vgl_xio_point_2d.txx vgl_xio_point_2d.h
  vgl_xio_point_3d.txx vgl_xio_point_3d.h
  vgl_xio_box_3d.txx vgl_xio_box_3d.h
  vgl_xio_vector_3d.txx vgl_xio_vector_3d.h
)

AUX_SOURCE_DIRECTORY(Templates vgl_xio_sources)

ADD_LIBRARY(vgl_xio ${vgl_xio_sources})
TARGET_LINK_LIBRARIES( vgl_xio vgl vsl bxml )

INSTALL_TARGETS(/lib vgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/core/vgl/xio ${vgl_xio_sources})

#IF( BUILD_TESTING)
#  SUBDIRS(tests)
#ENDIF( BUILD_TESTING)
