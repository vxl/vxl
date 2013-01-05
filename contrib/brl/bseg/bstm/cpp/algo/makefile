USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_cpp_algo

MANPAGE_SOURCES += bstm_data_similarity_traits.h
MANPAGE_SOURCES += bstm_ingest_boxm2_scene_function.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
