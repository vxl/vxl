USES += contrib/brl contrib/gel contrib/brl/bbas core

LIBRARY := bxml
VERSION := 1.0
DEFINES += -DBUILDING_BXML_DLL

SOURCES += bxml_document.cxx
SOURCES += bxml_find.cxx
SOURCES += bxml_read.cxx
SOURCES += bxml_write.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
