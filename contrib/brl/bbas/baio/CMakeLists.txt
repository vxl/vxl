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

vxl_add_library(LIBRARY_NAME baio LIBRARY_SOURCES ${baio_sources})

if(APPLE)
    target_link_libraries(baio c)
elseif(UNIX)
    target_link_libraries(baio rt)
endif()

#install the .h .hxx and libs

#tests
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
