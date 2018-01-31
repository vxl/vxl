# v3p/clipper/CMakeLists.txt
#
#   This is the clipper directory of v3p. Current version v6.4.2 (27 February 2017) Rev 512
#   See the README file for more details, or refer to
#   http://www.angusj.com/delphi/clipper.php

project( clipper CXX )

set( clipper_sources
 clipper.cpp clipper.hxx
)
vxl_add_library(LIBRARY_NAME clipper LIBRARY_SOURCES ${clipper_sources} )
