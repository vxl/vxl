# ./oxl/vrml/CMakeLists.txt

ADD_LIBRARY(vrml vrml_sources)

SOURCE_FILES(vrml_sources
  vrml_out.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/vrml/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
