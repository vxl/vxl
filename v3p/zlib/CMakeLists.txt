# ./v3p/zlib/CMakeLists.txt

ADD_LIBRARY( zlib zlib_sources )

SOURCE_FILES( zlib_sources
  adler32.c
  compress.c
  crc32.c
  gzio.c
  uncompr.c
  deflate.c
  trees.c
  zutil.c
  inflate.c
  infblock.c
  inftrees.c
  infcodes.c
  infutil.c
  inffast.c
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/v3p/zlib/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
