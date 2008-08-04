USES += contrib/mul core

LIBRARY := mipa
VERSION := 1.0

MANPAGE_SOURCES += mipa_orientation_histogram.txx
MANPAGE_SOURCES += mipa_sample_histo_boxes.txx
SOURCES += mipa_block_normaliser.cxx
SOURCES += mipa_identity_normaliser.cxx
SOURCES += mipa_l2norm_vector_normaliser.cxx
SOURCES += mipa_ms_block_normaliser.cxx
SOURCES += mipa_vector_normaliser.cxx
SOURCES += mipa_add_all_loaders.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tools tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
