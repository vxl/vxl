USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := sdet_pro
VERSION := 1.0

SOURCES += sdet_register.cxx

MANPAGE_SOURCES += sdet_processes.h
SOURCES += processes/sdet_detect_edges_process.cxx
SOURCES += processes/sdet_detect_edge_tangent_process.cxx
SOURCES += processes/sdet_detect_edge_tangent_interp_process.cxx
SOURCES += processes/sdet_detect_third_order_edges_process.cxx
SOURCES += processes/sdet_texture_classifier_process.cxx
SOURCES += processes/sdet_texture_classifier_kernel_margin_process.cxx
SOURCES += processes/sdet_create_texture_classifier_process.cxx
SOURCES += processes/sdet_texture_training_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
