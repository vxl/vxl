# v3p/zlib/CMakeLists.txt

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
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(deflate.c PROPERTIES COMPILE_FLAGS -O1)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY( zlib ${zlib_sources} )

ENDIF(NOT VXL_USING_NATIVE_ZLIB)

