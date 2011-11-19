USES += contrib/brl contrib/brl/bbas core

LIBRARY := bpgl_algo
VERSION := 1.0

SOURCES += bpgl_construct_cameras.cxx
SOURCES += bpgl_fm_compute_affine_ransac.cxx
SOURCES += bpgl_camera_from_box.cxx
SOURCES += bpgl_interpolate.cxx
SOURCES += bpgl_fm_compute_reg_ransac.cxx
SOURCES += bpgl_nitf_camera_coverage.cxx
SOURCES += bpgl_fm_compute_ransac.cxx
SOURCES += bpgl_project.cxx
SOURCES += bpgl_camera_homographies.cxx
SOURCES += bpgl_vsol_lens_warp.cxx
SOURCES += bpgl_bundle_rolling_shutter_adjust.cxx
MANPAGE_SOURCES += bpgl_lens_warp_mapper.h
MANPAGE_SOURCES += bpgl_list.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
