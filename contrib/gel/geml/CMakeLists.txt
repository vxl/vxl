# ./gel/geml/CMakeLists.txt

ADD_LIBRARY(geml geml_sources)

SOURCE_FILES( geml_sources
  geml_matcher.cxx
  geml_matcher_correlation.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/geml/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
