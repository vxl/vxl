# v3p/zlib/CMakeLists.txt

PROJECT( zlib C )

INCLUDE(${MODULE_PATH}/FindZLIB.cmake)

IF(NOT VXL_USING_NATIVE_ZLIB)

SET( zlib_sources
  zlib.h
  zconf.h

  adler32.c
  compress.c
  crc32.c
  gzio.c
  uncompr.c
  deflate.c deflate.h
  trees.c trees.h
  zutil.c zutil.h
  inflate.c
  infblock.c infblock.h
  inftrees.c inftrees.h
  infcodes.c infcodes.h
  infutil.c infutil.h
  inffast.c inffast.h
            inffixed.h

#minizip package to zip and unzip files
  minizip/crypt.h
  minizip/ioapi.c      minizip/ioapi.h
  minizip/zip.h        minizip/zip.c
  minizip/unzip.c      minizip/unzip.h      
  minizip/mztools.c    minizip/mztools.h
  
  minizip/minizip.c
  minizip/miniunz.c
)

IF(WIN32)
  SET( zlib_sources ${zlib_sources}
       minizip/iowin32.c    minizip/iowin32.h)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(deflate.c PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY( z ${zlib_sources} )
INSTALL_TARGETS( /lib z )

ENDIF(NOT VXL_USING_NATIVE_ZLIB)

