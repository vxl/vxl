USES += contrib/brl contrib/brl/bbas contrib/brl/b3p core

LIBRARY := bnl-algo
VERSION := 1.0

SOURCES += bnl_legendre_polynomial.cxx
SOURCES += bnl_simpson_integral.cxx
SOURCES += bnl_adaptsimpson_integral.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
