# ./gel/gtrl/CMakeLists.txt

ADD_LIBRARY(gtrl gtrl_sources)

SOURCE_FILES( gtrl_sources
  gtrl_triangulation.cxx
  gtrl_triangle.cxx
  gtrl_vertex.cxx
  gtrl_polygon.cxx
)

AUX_SOURCE_DIRECTORY(Templates gtrl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/gtrl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
