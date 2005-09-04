USES += contrib/mul core
include ../config.mk
include ${VXLROOT}/contrib/mul/vpdfl/config.mk
include ${VXLROOT}/core/vul/config.mk
include $(VXLROOT)/core/vpl/config.mk
include ${VXLROOT}/core/vbl/config.mk
include $(VXLROOT)/core/testlib/config.mk

TESTMAIN := test_driver.cxx
TESTS += test_jarque_bera.cxx
TESTS += test_parse_block.cxx
TESTS += test_lda.cxx
TESTS += test_file_data_wrapper.cxx
TESTS += test_selected_data_wrapper.cxx
TESTS += test_priority_bounded_queue.cxx
TESTS += test_stochastic_data_collector.cxx
TESTS += test_k_means.cxx
TESTS += test_matrix_products.cxx
TESTS += test_matxvec.cxx
TESTS += test_stats_1d.cxx
TESTS += test_sum_1d.cxx
TESTS += test_gamma.cxx
TESTS += test_index_sort.cxx
TESTS += test_lru_cache.cxx
TESTS += test_clamped_plate_spline_2d.cxx
TESTS += test_histogram.cxx
TESTS += test_data_wrapper_mixer.cxx
TESTS += test_combination.cxx
TESTS += test_ar_process.cxx
TESTS += test_random_n_from_m.cxx
TESTS += test_select_n_from_m.cxx
TESTS += test_thin_plate_spline_2d.cxx
TESTS += test_thin_plate_spline_3d.cxx
TESTS += test_thin_plate_spline_weights_3d.cxx
TESTS += test_read_props.cxx
TESTS += test_correspond_points.cxx
TESTS += test_rbf_network.cxx
TESTS += test_stats_nd.cxx
TESTS += test_table.cxx
TESTS += test_cloneables_factory.cxx
TESTS += test_rvm_regression_builder.cxx
TESTS += test_test.cxx

MINI_PROG_SOURCES += test_include.cxx

#########################################
include ${VXLROOT}/config/static-tests.mk
#########################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_test_mbl.cxx := -O0
endif
