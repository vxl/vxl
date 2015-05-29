# v3p/clipper/CMakeLists.txt
#
#   This is the clipper directory of v3p.  Current version is 6.1.3a.
#   See the README file for more details, or refer to 
#   http://www.angusj.com/delphi/clipper.php

PROJECT( clipper CXX )

SET( clipper_sources
     clipper.cpp clipper.hpp
)

ADD_DEFINITIONS( -D_FILE_OFFSET_BITS )

ADD_LIBRARY( clipper ${clipper_sources} )
INSTALL_TARGETS( /lib clipper )
