# ./gel/gkll/CMakeLists.txt

ADD_LIBRARY(gkll gkll_sources)

SOURCE_FILES( gkll_sources
  gkll_kl.cxx
  gkll_kl_params.cxx
  gkll_multi_view_data.cxx
)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/gkll/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)
