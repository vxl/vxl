USES := vxl
USES += mul

LIBRARY := mbl
VERSION := 1.0

SOURCES += mbl_gamma.cxx
SOURCES += mbl_mz_random.cxx
SOURCES += mbl_matxvec.cxx
SOURCES += mbl_matrix_products.cxx
SOURCES += mbl_data_collector_base.cxx
SOURCES += mbl_k_means.cxx
SOURCES += mbl_read_props.cxx
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
SOURCES += mbl_histogram.cxx
SOURCES += mbl_clamped_plate_spline_2d.cxx
SOURCES += mbl_read_yes.cxx

# Additional files to document:
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

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
