USES += contrib/brl contrib/brl/bseg core

LIBRARY := brip
VERSION := 1.0

MANPAGE_SOURCES += brip_histogram.txx
MANPAGE_SOURCES += brip_mutual_info.txx
SOURCES += brip_float_ops.cxx
MANPAGE_SOURCES += brip_vil_ops.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
