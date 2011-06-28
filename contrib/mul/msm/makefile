USES += contrib/mul core

LIBRARY := msm
VERSION := 1.0

SOURCES += msm_add_all_loaders.cxx

SOURCES += msm_points.cxx
SOURCES += msm_similarity_aligner.cxx
SOURCES += msm_ref_shape_instance.cxx
SOURCES += msm_no_limiter.cxx
SOURCES += msm_wt_mat_2d.cxx
SOURCES += msm_translation_aligner.cxx
SOURCES += msm_shape_model_builder.cxx
SOURCES += msm_param_limiter.cxx
SOURCES += msm_shape_instance.cxx
SOURCES += msm_aligner.cxx
SOURCES += msm_curve.cxx
SOURCES += msm_shape_model.cxx
SOURCES += msm_box_limiter.cxx
SOURCES += msm_zoom_aligner.cxx
SOURCES += msm_ellipsoid_limiter.cxx
SOURCES += msm_ref_shape_model.cxx
SOURCES += msm_shape_perturber.cxx
SOURCES += msm_reflect_shape.cxx

SUBDIRS += tools utils

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
