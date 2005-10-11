# conversions/container/CMakeLists.txt

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

SET(container_sources
 CoolArray_to_CoolList.h
 CoolArray_to_vcl_list.h
 CoolArray_to_vcl_vector.h
 CoolList_to_CoolArray.h
 CoolList_to_vcl_list.h
 CoolList_to_vcl_vector.h
 vcl_list_to_CoolArray.h
 vcl_list_to_CoolList.h
 vcl_list_to_vcl_vector.h
 vcl_vector_to_CoolArray.h
 vcl_vector_to_CoolList.h
 vcl_vector_to_vcl_list.h
 vcl_vector_to_vnl_vector.h
 vnl_vector_to_vcl_vector.h
)

INSTALL_NOBASE_HEADER_FILES(/include/contrib/conversions/container ${container_sources})
