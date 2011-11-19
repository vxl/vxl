USES += core

LIBRARY := vpgl-io
VERSION := 1.0

SOURCES += vpgl_io_lvcs.cxx
SOURCES += vpgl_io_camera.cxx
MANPAGE_SOURCES += vpgl_io_camera.txx
MANPAGE_SOURCES += vpgl_io_affine_camera.txx
MANPAGE_SOURCES += vpgl_io_rational_camera.txx
MANPAGE_SOURCES += vpgl_io_proj_camera.txx
MANPAGE_SOURCES += vpgl_io_local_rational_camera.txx
MANPAGE_SOURCES += vpgl_io_perspective_camera.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
