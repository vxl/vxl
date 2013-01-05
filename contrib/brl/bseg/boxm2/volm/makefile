USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_volm

SOURCES += boxm2_volm_matcher.cxx
SOURCES += boxm2_volm_wr3db_index.cxx
SOURCES += boxm2_volm_locations.cxx
MANPAGE_SOURCES += boxm2_volm_locations_sptr.h
MANPAGE_SOURCES += boxm2_volm_wr3db_index_sptr.h
SOURCES += boxm2_volm_matcher_order.cxx
MANPAGE_SOURCES += boxm2_volm_io.h

SUBDIRS += io pro exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
