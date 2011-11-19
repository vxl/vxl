USES += core

LIBRARY := vpgl
VERSION := 1.0

SOURCES += vpgl_datum_conversion.cxx
SOURCES += vpgl_utm.cxx
SOURCES += vpgl_lvcs.cxx
MANPAGE_SOURCES += vpgl_earth_constants.h
MANPAGE_SOURCES += vpgl_camera.h
MANPAGE_SOURCES += vpgl_dll.h

MANPAGE_SOURCES += vpgl_calibration_matrix.txx
MANPAGE_SOURCES += vpgl_essential_matrix.txx
MANPAGE_SOURCES += vpgl_generic_camera.txx
MANPAGE_SOURCES += vpgl_proj_camera.txx
MANPAGE_SOURCES += vpgl_fundamental_matrix.txx
MANPAGE_SOURCES += vpgl_perspective_camera.txx
MANPAGE_SOURCES += vpgl_affine_fundamental_matrix.txx
MANPAGE_SOURCES += vpgl_rational_camera.txx
MANPAGE_SOURCES += vpgl_affine_camera.txx
MANPAGE_SOURCES += vpgl_local_rational_camera.txx

MANPAGE_SOURCES += vpgl_camera_double_sptr.h
MANPAGE_SOURCES += vpgl_camera_float_sptr.h
MANPAGE_SOURCES += vpgl_lvcs_sptr.h

SUBDIRS += file_formats algo io xio

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
