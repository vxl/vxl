USES += contrib/brl contrib/brl/bbas core

LIBRARY := bsta-io
VERSION := 1.0

MANPAGE_SOURCES += bsta_io_attributes.h
MANPAGE_SOURCES += bsta_io_gaussian_full.h
MANPAGE_SOURCES += bsta_io_gaussian_indep.h
MANPAGE_SOURCES += bsta_io_gaussian_sphere.h
MANPAGE_SOURCES += bsta_io_mixture.h
MANPAGE_SOURCES += bsta_io_mixture_fixed.h
SOURCES += bsta_io_histogram.cxx
MANPAGE_SOURCES += bsta_io_parzen_sphere.h
MANPAGE_SOURCES += bsta_io_von_mises.h
MANPAGE_SOURCES += bsta_io_beta.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
