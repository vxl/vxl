USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := vpgl_io
VERSION := 1.0

MANPAGE_SOURCES += vpgl_io_camera.txx
MANPAGE_SOURCES += vpgl_io_proj_camera.h
MANPAGE_SOURCES += vpgl_io_rational_camera.h
MANPAGE_SOURCES += vpgl_io_local_rational_camera.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
