#
# ./mul/mbl/CMakeLists.txt
#
#   Ian Scott
#



ADD_LIBRARY(mbl mbl_sources)

SOURCE_FILES(mbl_sources
mbl_gamma
mbl_matrix_products
mbl_matxvec
mbl_mz_random
mbl_k_means
mbl_data_collector_base
mbl_stats_1d
mbl_sum_1d
mbl_read_props
)

AUX_SOURCE_DIRECTORY(Templates mbl_sources)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
