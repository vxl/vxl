USES += core

LIBRARY := vpgl_algo
VERSION := 1.0

SOURCES += vpgl_fm_compute_7_point.cxx
SOURCES += vpgl_rational_adjust_multipt.cxx
SOURCES += vpgl_invmap_cost_function.cxx
SOURCES += vpgl_rational_adjust_onept.cxx
SOURCES += vpgl_ortho_procrustes.cxx
SOURCES += vpgl_triangulate_points.cxx
SOURCES += vpgl_camera_compute.cxx
SOURCES += vpgl_bundle_adjust_lsqr.cxx
SOURCES += vpgl_optimize_camera.cxx
SOURCES += vpgl_rational_adjust.cxx
SOURCES += vpgl_ray_intersect.cxx
SOURCES += vpgl_camera_convert.cxx
SOURCES += vpgl_ba_shared_k_lsqr.cxx
SOURCES += vpgl_backproject.cxx
SOURCES += vpgl_bundle_adjust.cxx
SOURCES += vpgl_ba_fixed_k_lsqr.cxx
SOURCES += vpgl_ray.cxx
SOURCES += vpgl_fm_compute_2_point.cxx
SOURCES += vpgl_camera_homographies.cxx
SOURCES += vpgl_fm_compute_8_point.cxx
MANPAGE_SOURCES += vpgl_em_compute_5_point.txx

SUBDIRS += examples

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
