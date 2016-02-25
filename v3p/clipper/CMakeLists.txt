# v3p/clipper/CMakeLists.txt
#
#   This is the clipper directory of v3p.  Current version is 6.1.3a.
#   See the README file for more details, or refer to
#   http://www.angusj.com/delphi/clipper.php

project( clipper CXX )

if( NOT CMAKE_SIZEOF_VOID_P EQUAL 8 )
    add_definitions( -Duse_int32 )
endif()

set( clipper_sources
     clipper.cpp clipper.hxx
)

add_definitions( -D_FILE_OFFSET_BITS )

vxl_add_library( LIBNAME clipper LIBSRCS ${clipper_sources} )
