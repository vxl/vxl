#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl rpl

LIBRARY := rsdl
VERSION := 1.0

MANPAGE_SOURCES += rsdl_fwd.h

SOURCES += rsdl_bounding_box.cxx
SOURCES += rsdl_dist.cxx
SOURCES += rsdl_kd_tree.cxx
SOURCES += rsdl_point.cxx
MANPAGE_SOURCES += rsdl_bins_2d.txx
MANPAGE_SOURCES += rsdl_borgefors.txx

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
