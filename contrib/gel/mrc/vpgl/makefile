USES += contrib/gel contrib/gel/mrc core

LIBRARY := vpgl
VERSION := 1.0

MANPAGE_SOURCES += vpgl_camera.h
#SOURCES += vpgl_basic_camera.cxx
#SOURCES += vpgl_matrix_camera.cxx
#SOURCES += vpgl_perspective_camera.cxx
MANPAGE_SOURCES += vpgl_perspective_camera.txx
#SOURCES += vpgl_rational_camera.cxx
MANPAGE_SOURCES += vpgl_calibration_matrix.txx
MANPAGE_SOURCES += vpgl_fundamental_matrix.txx
MANPAGE_SOURCES += vpgl_lens_distortion.txx
MANPAGE_SOURCES += vpgl_poly_radial_distortion.txx
MANPAGE_SOURCES += vpgl_proj_camera.txx
MANPAGE_SOURCES += vpgl_radial_distortion.txx
MANPAGE_SOURCES += vpgl_reg_fundamental_matrix.txx
MANPAGE_SOURCES += vpgl_affine_camera.txx
MANPAGE_SOURCES += vpgl_affine_fundamental_matrix.txx
MANPAGE_SOURCES += vgl_rotation_3d.h	is missing from contrib/gel/mrc/vpgl/makefile

#MANPAGE_SOURCES += vpgl_basic_camera_sptr.h
MANPAGE_SOURCES += vpgl_matrix_camera_sptr.h
MANPAGE_SOURCES += vpgl_perspective_camera_sptr.h

SUBDIRS += algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

