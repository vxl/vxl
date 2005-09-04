# writen by J.L. Mundy for basic dynamic programming classes
USES += contrib/brl contrib/brl/bbas core

LIBRARY := bdpg
VERSION := 1.0

SOURCES += bdpg_node.cxx
MANPAGE_SOURCES += bdpg_node_sptr.h
SOURCES += bdpg_array_dynamic_prg.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
