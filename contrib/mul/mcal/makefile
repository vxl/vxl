USES += contrib/mul core

LIBRARY := mcal
VERSION := 1.0

SOURCES += mcal_add_all_loaders.cxx
SOURCES += mcal_component_analyzer.cxx
SOURCES += mcal_pca.cxx
SOURCES += mcal_single_basis_cost.cxx
SOURCES += mcal_sparse_basis_cost.cxx
SOURCES += mcal_general_ca.cxx
SOURCES += mcal_trivial_ca.cxx
SOURCES += mcal_var_basis_cost.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
