# vxl/vnl/xio/CMakeLists.txt

set(vnl_xio_sources
  vnl_xio_matrix_fixed.txx   vnl_xio_matrix_fixed.h
  vnl_xio_matrix.txx         vnl_xio_matrix.h
  vnl_xio_vector_fixed.txx   vnl_xio_vector_fixed.h
  vnl_xio_vector.txx         vnl_xio_vector.h
  vnl_xio_quaternion.txx     vnl_xio_quaternion.h
)

aux_source_directory(Templates vnl_xio_sources)

add_library(vnl_xio ${vnl_xio_sources} )
target_link_libraries( vnl_xio vnl vsl )

install(TARGETS vnl_xio DESTINATION lib)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vnl/xio ${vnl_xio_sources})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
