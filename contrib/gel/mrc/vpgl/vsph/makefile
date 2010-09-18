USES += contrib/gel contrib/gel/mrc core

LIBRARY := vsph
VERSION := 1.0

SOURCES += vsph_spherical_coord.cxx
MANPAGE_SOURCES += vsph_view_sphere.txx
MANPAGE_SOURCES += vsph_view_point.h

MANPAGE_SOURCES += vsph_spherical_coord_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
