# ./gel/gkll/CMakeLists.txt
#

ADD_LIBRARY(gkll gkll_sources)

SOURCE_FILES( gkll_sources
  gkll_kl
  gkll_kl_params
  gkll_multi_view_data
)

INCLUDE_DIRECTORIES(${allvxl_SOURCE_DIR}/oxl)
