# v3p/zlib/CMakeLists.txt

project( zlib C )

set( zlib_sources
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


if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(deflate.c PROPERTIES COMPILE_FLAGS -O1)
endif()

vxl_add_library(LIBRARY_NAME z LIBRARY_SOURCES ${zlib_sources} )

