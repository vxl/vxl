USES += contrib/brl contrib/brl/bbas core v3p/netlib

LIBRARY := bnl-algo
VERSION := 1.0

SOURCES += bnl_legendre_polynomial.cxx
SOURCES += bnl_sparse_lm.cxx

#SUBDIRS += sparse

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
