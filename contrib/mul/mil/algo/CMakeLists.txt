# ./mul/mil/algo/CMakeLists.txt

ADD_LIBRARY(mil_algo mil_algo_sources)

SOURCE_FILES(mil_algo_sources
  mil_algo_exp_filter_1d.cxx
  mil_algo_gaussian_filter.cxx
  mil_algo_grad_3x3.cxx
  mil_algo_grad_1x3.cxx
)

AUX_SOURCE_DIRECTORY(Templates mil_algo_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE( ${allvxl_SOURCE_DIR}/mul/mil/algo/CMakeListsHeaders.txt )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
