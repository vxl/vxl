USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta
VERSION := 1.0

SOURCES += bsta_gauss.cxx
MANPAGE_SOURCES += bsta_histogram.txx
MANPAGE_SOURCES += bsta_joint_histogram.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
