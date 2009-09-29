USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvpl_util
VERSION := 1.0

SOURCES += bvpl_corner_pair_finder.cxx
SOURCES += bvpl_corner_search_functor.cxx

SUBDIRS += io

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
