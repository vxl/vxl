USES += contrib/brl contrib/brl/bbas core

LIBRARY := brad
VERSION := 1.0

SOURCES += brad_illum_util.cxx
MANPAGE_SOURCES += brad_illum_cost_function.h
SOURCES += brad_sun_dir_index.cxx
SOURCES += brad_sun_pos.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
