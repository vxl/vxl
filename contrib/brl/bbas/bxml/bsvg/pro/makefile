USES += contrib/brl core

LIBRARY := bsvg_pro
VERSION := 1.0

SOURCES += bsvg_register.cxx
MANPAGE_SOURCES += bsvg_processes.h
SOURCES += processes/bsvg_plot_roc_process.cxx
SOURCES += processes/bsvg_plot_bar_processes.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
