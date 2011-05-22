# b3p/minizip/CMakeLists.txt
# minizip package to zip and unzip files

PROJECT( minizip C )

INCLUDE( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
IF(ZLIB_FOUND)
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )


SET( minizip_sources
  crypt.h
  ioapi.c      ioapi.h
  zip.h        zip.c
  unzip.c      unzip.h
  mztools.c    mztools.h
)

IF(WIN32)
  SET( minizip_sources ${minizip_sources}
       iowin32.c    iowin32.h)
ENDIF(WIN32)

ADD_LIBRARY( minizip ${minizip_sources} )
INSTALL_TARGETS( /lib minizip )

ENDIF(ZLIB_FOUND)
