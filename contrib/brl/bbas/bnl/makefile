USES += contrib/brl contrib/brl/bbas core

LIBRARY := bnl
VERSION := 1.0

SOURCES += bnl_parabolic_interpolator.cxx
SOURCES += bnl_quadratic_interpolator.cxx

SUBDIRS += algo

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
