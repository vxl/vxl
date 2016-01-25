# vxl/vnl/io/CMakeLists.txt

set(vnl_io_sources
  vnl_io_nonlinear_minimizer.cxx  vnl_io_nonlinear_minimizer.h
  vnl_io_real_npolynomial.cxx     vnl_io_real_npolynomial.h
  vnl_io_real_polynomial.cxx      vnl_io_real_polynomial.h
  vnl_io_rational.cxx             vnl_io_rational.h
  vnl_io_bignum.cxx               vnl_io_bignum.h

  vnl_io_diag_matrix.txx          vnl_io_diag_matrix.h
  vnl_io_matrix.txx               vnl_io_matrix.h
  vnl_io_sparse_matrix.txx        vnl_io_sparse_matrix.h
  vnl_io_sym_matrix.txx           vnl_io_sym_matrix.h
  vnl_io_vector.txx               vnl_io_vector.h
  vnl_io_vector_fixed.txx         vnl_io_vector_fixed.h

  vnl_io_matrix_fixed.txx         vnl_io_matrix_fixed.h
)

aux_source_directory(Templates vnl_io_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(
    Templates/vsl_vector_io+vcl_vector+vcl_vector+vnl_vector+double----.cxx
    PROPERTIES
    COMPILE_FLAGS -ftemplate-depth-50)
endif()

add_library(vnl_io ${vnl_io_sources} )
target_link_libraries( vnl_io vnl vsl )

install(TARGETS vnl_io DESTINATION lib)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vnl/io ${vnl_io_sources})

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
