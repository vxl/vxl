USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := ihog_pro
VERSION := 1.0

MANPAGE_SOURCES += ihog_processes.h
SOURCES += ihog_register.cxx

SOURCES += processes/ihog_register_translational_process.cxx
SOURCES += processes/ihog_mutual_information_process.cxx
SOURCES += processes/ihog_compute_mi_cost_surface_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
