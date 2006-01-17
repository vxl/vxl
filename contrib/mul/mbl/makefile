USES += contrib/mul core

LIBRARY := mbl
VERSION := 1.0

SOURCES += mbl_add_all_loaders.cxx
SOURCES += mbl_gamma.cxx
SOURCES += mbl_matxvec.cxx
SOURCES += mbl_matrix_products.cxx
SOURCES += mbl_data_collector_base.cxx
SOURCES += mbl_k_means.cxx
SOURCES += mbl_read_props.cxx
SOURCES += mbl_read_multi_props.cxx
SOURCES += mbl_stats_1d.cxx
SOURCES += mbl_sum_1d.cxx
SOURCES += mbl_lda.cxx
SOURCES += mbl_screen_counter.cxx
SOURCES += mbl_rbf_network.cxx
SOURCES += mbl_read_double.cxx
SOURCES += mbl_read_int.cxx
SOURCES += mbl_read_str.cxx
#SOURCES += mbl_arb_length_int.cxx
SOURCES += mbl_thin_plate_spline_2d.cxx
SOURCES += mbl_thin_plate_spline_3d.cxx
SOURCES += mbl_thin_plate_spline_weights_3d.cxx
SOURCES += mbl_histogram.cxx
SOURCES += mbl_clamped_plate_spline_2d.cxx
SOURCES += mbl_read_yes.cxx
SOURCES += mbl_random_n_from_m.cxx
SOURCES += mbl_select_n_from_m.cxx
SOURCES += mbl_parse_block.cxx
SOURCES += mbl_correspond_points.cxx
SOURCES += mbl_stats_nd.cxx
SOURCES += mbl_table.cxx
SOURCES += mbl_progress.cxx
SOURCES += mbl_progress_composite.cxx
SOURCES += mbl_progress_null.cxx
SOURCES += mbl_progress_text.cxx
SOURCES += mbl_rvm_regression_builder.cxx
SOURCES += mbl_exception.cxx
SOURCES += mbl_test.cxx
SOURCES += mbl_log.cxx

# Additional files to document:
MANPAGE_SOURCES += mbl_cloneables_factory.h
MANPAGE_SOURCES += mbl_data_array_ptr_wrapper.txx
MANPAGE_SOURCES += mbl_data_array_wrapper.txx
MANPAGE_SOURCES += mbl_data_collector.txx
MANPAGE_SOURCES += mbl_data_collector_list.txx
MANPAGE_SOURCES += mbl_data_wrapper.txx
MANPAGE_SOURCES += mbl_data_wrapper_mixer.txx
MANPAGE_SOURCES += mbl_stochastic_data_collector.txx
MANPAGE_SOURCES += mbl_selected_data_wrapper.txx
MANPAGE_SOURCES += mbl_chord.h
MANPAGE_SOURCES += mbl_cloneable_ptr.h
MANPAGE_SOURCES += mbl_index_sort.h
MANPAGE_SOURCES += mbl_lru_cache.h
MANPAGE_SOURCES += mbl_print.h
MANPAGE_SOURCES += mbl_priority_bounded_queue.h
MANPAGE_SOURCES += mbl_stl.h
MANPAGE_SOURCES += mbl_file_data_collector.txx
MANPAGE_SOURCES += mbl_file_data_wrapper.txx
MANPAGE_SOURCES += mbl_combination.h
MANPAGE_SOURCES += mbl_ar_process.txx
MANPAGE_SOURCES += mbl_jarque_bera.h
MANPAGE_SOURCES += mbl_stl_pred.h
MANPAGE_SOURCES += mbl_chord_3d.h

MANPAGE_SOURCES += mbl_config.h.in

# This is where configure will put mbl/mbl_config.h
INCDIRS += $(RELOBJDIR)/include

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

# configure stuff:
xall:: $(RELOBJDIR)/include/mbl/mbl_config.h
$(RELOBJDIR)/include/mbl/mbl_config.h: mbl_config.h.in
	mkdir -p $(RELOBJDIR)/include/mbl
	sed -e 's/@HAS_.*@/1/' -e 's/[._]in//g' $^ > $@

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
