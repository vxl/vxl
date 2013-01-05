USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_io

SOURCES += bstm_cache.cxx
SOURCES += bstm_lru_cache.cxx
SOURCES += bstm_sio_mgr.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
