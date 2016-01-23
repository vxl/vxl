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

add_library(vpl ${vpl_sources})
if(NOT WIN32)
  set_target_properties(vpl PROPERTIES COMPILE_FLAGS "-fPIC")
endif(NOT WIN32)

target_link_libraries( vpl vcl )


if(NOT UNIX)
  target_link_libraries( vpl ws2_32 vcl )
endif(NOT UNIX)

install_targets(/lib vpl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpl ${vpl_sources})

if( BUILD_TESTING)
  add_subdirectory(tests)
endif( BUILD_TESTING)
