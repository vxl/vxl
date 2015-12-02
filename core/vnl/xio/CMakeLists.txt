# vxl/vnl/xio/CMakeLists.txt

SET(vnl_xio_sources
  vnl_xio_matrix_fixed.txx   vnl_xio_matrix_fixed.h
  vnl_xio_matrix.txx         vnl_xio_matrix.h
  vnl_xio_vector_fixed.txx   vnl_xio_vector_fixed.h
  vnl_xio_vector.txx         vnl_xio_vector.h
  vnl_xio_quaternion.txx     vnl_xio_quaternion.h
)

AUX_SOURCE_DIRECTORY(Templates vnl_xio_sources)

ADD_LIBRARY(vnl_xio ${vnl_xio_sources} )
TARGET_LINK_LIBRARIES( vnl_xio vnl vsl )

INSTALL(TARGETS vnl_xio DESTINATION lib)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vnl/xio ${vnl_xio_sources})

IF(BUILD_TESTING)
  add_subdirectory(tests)
ENDIF(BUILD_TESTING)
