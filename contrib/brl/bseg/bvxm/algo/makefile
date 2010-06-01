USES += contrib/brl contrib/brl/bbas core

LIBRARY := bvxm_algo

SOURCES += bvxm_merge_mog.cxx
MANPAGE_SOURCES += bvxm_mog_norm.txx
MANPAGE_SOURCES += bvxm_boxm_convert.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
