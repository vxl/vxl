USES += contrib/brl contrib/brl/bbas core

LIBRARY := bnl
VERSION := 1.0

SOURCES += bnl_parabolic_interpolator.cxx
SOURCES += bnl_quadratic_interpolator.cxx
SOURCES += bnl_crs_index.cxx
SOURCES += bnl_sparse_lst_sqr_function.cxx

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
