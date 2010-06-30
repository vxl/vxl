# v3p/bzlib/CMakeLists.txt
#
#   This is the bzlib2 directory of v3p.  Current version is 1.0.5.
#   See the README file for more details, or refer to http://bzip.org/

PROJECT( bzlib C )

INCLUDE(${CMAKE_ROOT}/Modules/FindBZip2.cmake)

IF(NOT VXL_USING_NATIVE_BZLIB2)

SET( bzlib2_sources
     blocksort.c
     huffman.c
     crctable.c
     randtable.c
     compress.c
     decompress.c
     bzlib.c      bzlib.h  bzlib_private.h
)

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS -DBZ_DEBUG=0 )

ADD_LIBRARY( bzip2 ${bzlib2_sources} )
INSTALL_TARGETS( /lib bzip2 )

ENDIF(NOT VXL_USING_NATIVE_BZLIB2)
