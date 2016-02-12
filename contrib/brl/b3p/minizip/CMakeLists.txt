# b3p/minizip/CMakeLists.txt
# minizip package to zip and unzip files

project( minizip C )

include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
if(ZLIB_FOUND)
  include_directories( ${ZLIB_INCLUDE_DIR} )


set( minizip_sources
  crypt.h
  ioapi.c      ioapi.h
  zip.h        zip.c
  unzip.c      unzip.h
  mztools.c    mztools.h
)

if(WIN32)
  set( minizip_sources ${minizip_sources}
       iowin32.c    iowin32.h)
endif()

add_library( minizip ${minizip_sources} )
target_link_libraries(minizip z)
install_targets( /lib minizip )

endif()
