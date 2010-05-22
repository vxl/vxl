USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

SOURCES += boxm_raytrace_operations.cxx
SOURCES += boxm_triangle_scan_iterator.cxx
SOURCES += boxm_rational_camera_utils.cxx
SOURCES += boxm_quad_scan_iterator.cxx
SOURCES += boxm_utils.cxx
SOURCES += boxm_sigma_normalizer.cxx
SOURCES += boxm_merge_mog.cxx
SOURCES += boxm_test_util.cxx

MANPAGE_SOURCES += boxm_triangle_interpolation_iterator.txx
MANPAGE_SOURCES += boxm_vrml_util.h
MANPAGE_SOURCES += boxm_plane_ransac.h
MANPAGE_SOURCES += boxm_cell_data_traits.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
