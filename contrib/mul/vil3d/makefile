USES += contrib/mul core

LIBRARY := vil3d
VERSION := 1.0

SOURCES += vil3d_image_view_base.cxx
MANPAGE_SOURCES += vil3d_image_view.txx
MANPAGE_SOURCES += vil3d_plane.h
MANPAGE_SOURCES += vil3d_print.h
MANPAGE_SOURCES += vil3d_sample_profile_trilin.txx
MANPAGE_SOURCES += vil3d_slice.h
MANPAGE_SOURCES += vil3d_trilin_interp.h
MANPAGE_SOURCES += vil3d_crop.h

SUBDIRS += algo io

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
