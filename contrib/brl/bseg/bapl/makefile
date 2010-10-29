USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bapl

SOURCES += bapl_lowe_pyramid_set.cxx
SOURCES += bapl_affine2d_est.cxx
SOURCES += bapl_lowe_keypoint.cxx
SOURCES += bapl_keypoint_set.cxx
SOURCES += bapl_mi_matcher.cxx
SOURCES += bapl_connectivity.cxx
SOURCES += bapl_keypoint.cxx
SOURCES += bapl_lowe_cluster.cxx
SOURCES += bapl_affine_transform.cxx
SOURCES += bapl_affine_roi.cxx
SOURCES += bapl_mi_matcher_params.cxx
SOURCES += bapl_bbf_tree.cxx
SOURCES += bapl_keypoint_extractor.cxx
MANPAGE_SOURCES += bapl_lowe_pyramid.txx

MANPAGE_SOURCES += bapl_lowe_pyramid_set_sptr.h
MANPAGE_SOURCES += bapl_keypoint_set_sptr.h
MANPAGE_SOURCES += bapl_keypoint_sptr.h
MANPAGE_SOURCES += bapl_lowe_keypoint_sptr.h
MANPAGE_SOURCES += bapl_connectivity_sptr.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += examples
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
