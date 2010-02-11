USES += contrib/brl contrib/brl/bbas core

LIBRARY := boxm_opt

MANPAGE_SOURCES += boxm_opt_appearance_estimator.h
MANPAGE_SOURCES += boxm_aux_traits.h
MANPAGE_SOURCES += boxm_opt_rt_bayesian_optimizer.txx
MANPAGE_SOURCES += boxm_aux_scene.txx
MANPAGE_SOURCES += boxm_rt_sample.txx
MANPAGE_SOURCES += boxm_update_image_functor.h
MANPAGE_SOURCES += boxm_render_expected_image_functor.h
MANPAGE_SOURCES += boxm_raytrace_function.h
MANPAGE_SOURCES += boxm_render_expected_depth_functor.h
MANPAGE_SOURCES += boxm_generate_opt_sample_functor.h
MANPAGE_SOURCES += boxm_edge_updater.h
MANPAGE_SOURCES += boxm_seg_length_functor.h
MANPAGE_SOURCES += boxm_generate_edge_sample_functor.h
MANPAGE_SOURCES += boxm_pre_infinity_functor.h
MANPAGE_SOURCES += boxm_render_expected_edge_image_functor.h
MANPAGE_SOURCES += boxm_edge_updater.txx
MANPAGE_SOURCES += boxm_rpc_registration.h
MANPAGE_SOURCES += boxm_generate_edge_tangent_sample_functor.h
MANPAGE_SOURCES += boxm_render_expected_edge_tangent_image_functor.h
MANPAGE_SOURCES += boxm_edge_tangent_updater.txx

SUBDIRS += pro open_cl

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
