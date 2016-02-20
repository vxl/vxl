# conversions/container/CMakeLists.txt

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# These are the source files in this directory.
# Note that these are all header files, so no library is built here.

set(container_sources
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

INSTALL_NOBASE_HEADER_FILES(include/vxl/contrib/conversions/container ${container_sources})
