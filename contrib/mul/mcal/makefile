USES += contrib/mul core

LIBRARY := mcal
VERSION := 1.0

SOURCES += mcal_component_analyzer.cxx
SOURCES += mcal_pca.cxx
SOURCES += mcal_single_basis_cost.cxx
SOURCES += mcal_sparse_basis_cost.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
