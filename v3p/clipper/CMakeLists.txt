# v3p/clipper/CMakeLists.txt
#
#   This is the clipper directory of v3p.  Current version is 6.1.3a.
#   See the README file for more details, or refer to
#   http://www.angusj.com/delphi/clipper.php

project( clipper CXX )

if( NOT CMAKE_SIZEOF_VOID_P EQUAL 8 )
    add_definitions( -Duse_int32 )
endif( NOT CMAKE_SIZEOF_VOID_P EQUAL 8 )

set( clipper_sources
     clipper.cpp clipper.hpp
)

add_definitions( -D_FILE_OFFSET_BITS )

add_library( clipper ${clipper_sources} )
install_targets( /lib clipper )
