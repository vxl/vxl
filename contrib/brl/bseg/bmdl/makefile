USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bmdl

SOURCES += bmdl_mesh.cxx
MANPAGE_SOURCES += bmdl_classify.txx

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
