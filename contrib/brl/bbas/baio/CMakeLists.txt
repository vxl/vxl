# brl/bbas/baio/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR})
include_directories( ${BRL_INCLUDE_DIR}/bbas )

set(baio_sources
    baio.h
   )

if(WIN32)
  set(baio_sources ${baio_sources}
      baio_windows.cxx
     )
elseif(CYGWIN)
  set(baio_sources ${baio_sources}
      baio_windows.cxx
     )
elseif(APPLE)
  set(baio_sources ${baio_sources}
      baio_osx.cxx
     )
else()
  set(baio_sources ${baio_sources}
      baio_unix.cxx
     )
endif()

add_library(baio ${baio_sources})

if(APPLE)
    target_link_libraries(baio c)
elseif(UNIX)
    target_link_libraries(baio rt)
endif()

#install the .h .txx and libs
install_targets(/lib baio)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/baio/ ${baio_sources})

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
