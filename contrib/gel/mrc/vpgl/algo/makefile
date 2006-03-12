USES += contrib/gel core

LIBRARY := vpgl-algo
VERSION := 1.0

SOURCES += vpgl_camera_compute.cxx
SOURCES += vpgl_fm_compute_7_point.cxx
SOURCES += vpgl_fm_compute_8_point.cxx
SOURCES += vpgl_fm_compute_affine_ransac.cxx
SOURCES += vpgl_fm_compute_ransac.cxx
SOURCES += vpgl_fm_compute_reg_ransac.cxx
SOURCES += vpgl_list.cxx
SOURCES += vpgl_optimize_camera.cxx
SOURCES += vpgl_project.cxx
SOURCES += vpgl_vsol_lens_warp.cxx
MANPAGE_SOURCES += vpgl_lens_warp_mapper.h
MANPAGE_SOURCES += vpgl_construct_cameras.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
