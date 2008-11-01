USES += contrib/gel contrib/rpl contrib/gel/mrc core

LIBRARY := vpgl-algo
VERSION := 1.0

SOURCES += vpgl_camera_compute.cxx
SOURCES += vpgl_fm_compute_7_point.cxx
SOURCES += vpgl_fm_compute_8_point.cxx
SOURCES += vpgl_fm_compute_affine_ransac.cxx
SOURCES += vpgl_fm_compute_ransac.cxx
SOURCES += vpgl_fm_compute_reg_ransac.cxx
MANPAGE_SOURCES += vpgl_list.txx
SOURCES += vpgl_optimize_camera.cxx
SOURCES += vpgl_project.cxx
SOURCES += vpgl_backproject.cxx
SOURCES += vpgl_vsol_lens_warp.cxx
MANPAGE_SOURCES += vpgl_lens_warp_mapper.h
SOURCES += vpgl_construct_cameras.cxx
SOURCES += vpgl_invmap_cost_function.cxx
SOURCES += vpgl_fm_compute_2_point.cxx
SOURCES += vpgl_ortho_procrustes.cxx
SOURCES += vpgl_bundle_adjust.cxx
SOURCES += vpgl_adjust_rational_trans_onept.cxx
SOURCES += vpgl_camera_homographies.cxx
SOURCES += vpgl_interpolate.cxx
SOURCES += vpgl_rational_geo_adjust.cxx
SOURCES += vpgl_ray.cxx
SOURCES += vpgl_ray_intersect.cxx
SOURCES += vpgl_nitf_camera_coverage.cxx
SOURCES += vpgl_geo_camera.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
