# ./gel/vmal/CMakeLists.txt

ADD_LIBRARY(vmal vmal_sources)

SOURCE_FILES( vmal_sources
  vmal_convert.cxx
  vmal_dense_matching.cxx
  vmal_homog2d.cxx
  vmal_kl.cxx
  vmal_kl_params.cxx
  vmal_lines_correlation.cxx
  vmal_multi_view_data.cxx
  vmal_operators.cxx
  vmal_rectifier.cxx
  vmal_refine_lines.cxx
  vmal_track_lines.cxx
)

AUX_SOURCE_DIRECTORY(Templates vmal_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/gel/vmal/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)
