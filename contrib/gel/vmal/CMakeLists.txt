# ./gel/vmal/CMakeLists.txt
#

ADD_LIBRARY(vmal vmal_sources)

SOURCE_FILES( vmal_sources
  vmal_convert
  vmal_dense_matching
  vmal_homog2d
  vmal_kl
  vmal_kl_params
  vmal_lines_correlation
  vmal_multi_view_data
  vmal_operators
  vmal_rectifier
  vmal_refine_lines
  vmal_track_lines
)

AUX_SOURCE_DIRECTORY(Templates vmal_sources)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)
