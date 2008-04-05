USES += contrib/gel contrib/gel/mrc core

LIBRARY := bgeo
VERSION := 1.0

MANPAGE_SOURCES += bgeo_dll.h
SOURCES += bgeo_datum_conversion.cxx
SOURCES += bgeo_lvcs.cxx
MANPAGE_SOURCES += bgeo_lvcs_sptr.h
MANPAGE_SOURCES += bgeo_earth_constants.h
SOURCES += bgeo_utm.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
