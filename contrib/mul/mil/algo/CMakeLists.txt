# ./mul/mil/algo/CMakeLists.txt

ADD_LIBRARY(mil_algo mil_algo_sources)

SOURCE_FILES(mil_algo_sources
  mil_algo_exp_filter_1d.cxx
)

AUX_SOURCE_DIRECTORY(Templates mil_algo_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/algo/CMakeListsHeaders.txt )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)