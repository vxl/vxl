# vxl/vgl/xio/CMakeLists.txt

SET(vgl_xio_sources
  vgl_xio_point_2d.txx vgl_xio_point_2d.h
  vgl_xio_point_3d.txx vgl_xio_point_3d.h
  vgl_xio_box_3d.txx vgl_xio_box_3d.h
  vgl_xio_vector_3d.txx vgl_xio_vector_3d.h
)

AUX_SOURCE_DIRECTORY(Templates vgl_xio_sources)

ADD_LIBRARY(vgl_xio ${vgl_xio_sources})
IF(NOT WIN32)
  SET_TARGET_PROPERTIES(vgl_xio PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(NOT WIN32)
TARGET_LINK_LIBRARIES( vgl_xio vgl vsl )

INSTALL_TARGETS(/lib vgl_xio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vgl/xio ${vgl_xio_sources})

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
