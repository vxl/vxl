#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl rpl

LIBRARY := rsdl
VERSION := 1.0

SOURCES += rsdl_dist.cxx
SOURCES += rsdl_kd_tree.cxx
SOURCES += rsdl_point.cxx

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
