# v3p/bzlib/CMakeLists.txt
#
#   This is the bzlib2 directory of v3p.  Current version is 1.0.5.
#   See the README file for more details, or refer to http://bzip.org/

project( bzlib C )

find_package(BZip2)

if(NOT VXL_USING_NATIVE_BZLIB2)

set( bzlib2_sources
     blocksort.c
     huffman.c
     crctable.c
     randtable.c
     compress.c
     decompress.c
     bzlib.c      bzlib.h  bzlib_private.h
)

add_definitions( -D_FILE_OFFSET_BITS -DBZ_DEBUG=0 )

vxl_add_library(LIBRARY_NAME bzip2 LIBRARY_SOURCES ${bzlib2_sources} )

endif()
