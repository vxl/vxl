# ./vxl/vgl/algo/CMakeLists.txt

ADD_LIBRARY(vgl_algo vgl_algo_sources)

SOURCE_FILES( vgl_algo_sources
  vgl_algo.cxx
)

AUX_SOURCE_DIRECTORY(Templates vgl_algo_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vgl/algo/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)
