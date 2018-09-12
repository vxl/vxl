# ./vxl/vpl/CMakeLists.txt

doxygen_add_library(core/vpl
  PACKAGE core
  DESCRIPTION "POSIX Compatability Library"
  )

set( vpl_sources
  vpl.h         vpl.cxx
  vpl_fdopen.h  vpl_fdopen.cxx
  vpl_fileno.h  vpl_fileno.cxx
  vpl_mutex.h
)

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpl LIBRARY_SOURCES ${vpl_sources})
if(NOT WIN32)
  set(fPIC_COMPILER_FLAGS "-fPIC")
endif()
set(CURR_LIB_NAME vpl)
set_vxl_library_properties(
     COMPILE_FLAGS "${fPIC_COMPILER_FLAGS}"
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     USE_HIDDEN_VISIBILITY
)

target_link_libraries( ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vcl )
if(NOT UNIX)
  target_link_libraries( ${VXL_LIB_PREFIX}vpl ws2_32 ${VXL_LIB_PREFIX}vcl )
endif()

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vpl-obj OBJECT ${vpl_sources})
endif()
