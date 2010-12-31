USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2-basic

MANPAGE_SOURCES += boxm2_array_1d.txx
MANPAGE_SOURCES += boxm2_array_2d.txx
MANPAGE_SOURCES += boxm2_array_3d.txx
SOURCES += boxm2_block_id.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
