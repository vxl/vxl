# v3p/zlib/CMakeLists.txt

# Do nothing here, unless there is no native ZLIB
INCLUDE(${allvxl_SOURCE_DIR}/config.cmake/Modules/FindNativeZLIB.cmake)
IF(NOT HAS_NATIVE_ZLIB)

ADD_LIBRARY( zlib zlib_sources )

SOURCE_FILES( zlib_sources
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

ENDIF(NOT HAS_NATIVE_ZLIB)
