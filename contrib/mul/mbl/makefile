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

# Additional files to document:
MANPAGE_SOURCES += mbl_data_array_wrapper.txx
MANPAGE_SOURCES += mbl_data_collector.txx
MANPAGE_SOURCES += mbl_data_collector_list.txx
MANPAGE_SOURCES += mbl_data_wrapper.txx
MANPAGE_SOURCES += mbl_stochastic_data_collector.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
