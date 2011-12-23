USES += contrib/brl contrib/brl/bbas core

LIBRARY := bhdfs
VERSION := 1.0

SOURCES += bhdfs_manager.cxx
SOURCES += bhdfs_fstream.cxx
SOURCES += bhdfs_vil_save.cxx
SOURCES += bhdfs_vil_load.cxx
SOURCES += bhdfs_vil_stream.cxx

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
