# conversions/math/CMakeLists.txt

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

SET(math_sources
 CoolMatrix_to_vnl_matrix.h
 CoolVector_to_vnl_vector.h
 IUE_matrix_to_vnl_matrix.h
 IUE_vector_to_vnl_vector.h
 vbl_array_to_vnl_matrix.h
 vbl_array_to_vnl_vector.h
 vnl_matrix_to_CoolMatrix.h
 vnl_matrix_to_IUE_matrix.h
 vnl_matrix_to_vbl_array.h
 vnl_vector_to_CoolVector.h
 vnl_vector_to_IUE_vector.h
 vnl_vector_to_vbl_array.h
)

INSTALL_NOBASE_HEADER_FILES(/include/contrib/conversions/math ${math_sources})
