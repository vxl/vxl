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

vxl_add_library(LIBNAME ${VXL_LIB_PREFIX}vpl LIBSRCS ${vpl_sources})
if(NOT WIN32)
  set_target_properties(${VXL_LIB_PREFIX}vpl PROPERTIES COMPILE_FLAGS "-fPIC")
endif()

target_link_libraries( ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vcl )
if(NOT UNIX)
  target_link_libraries( ${VXL_LIB_PREFIX}vpl ws2_32 ${VXL_LIB_PREFIX}vcl )
endif()

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
