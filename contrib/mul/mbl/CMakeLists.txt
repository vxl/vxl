# ./mul/mbl/CMakeLists.txt

ADD_LIBRARY(mbl mbl_sources)

SOURCE_FILES(mbl_sources
  mbl_gamma.cxx
  mbl_matrix_products.cxx
  mbl_matxvec.cxx
  mbl_mz_random.cxx
  mbl_k_means.cxx
  mbl_data_collector_base.cxx
  mbl_stats_1d.cxx
  mbl_sum_1d.cxx
  mbl_read_props.cxx
  mbl_lda.cxx
  mbl_screen_counter.cxx
  mbl_rbf_network.cxx
  mbl_read_int.cxx
  mbl_read_double.cxx
  mbl_read_str.cxx
  mbl_arb_length_int.cxx
  mbl_thin_plate_spline_2d.cxx
)

AUX_SOURCE_DIRECTORY(Templates mbl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE( ${allvxl_SOURCE_DIR}/mul/mbl/CMakeListsHeaders.txt )
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)


IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
