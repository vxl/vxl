USES += contrib/rpl contrib/mul core

LIBRARY := rgrl
VERSION := 1.0

MANPAGE_SOURCES += rgrl_fwd.h
SOURCES += rgrl_object.cxx
MANPAGE_SOURCES += rgrl_object_sptr.h

SOURCES += rgrl_mask.cxx
MANPAGE_SOURCES += rgrl_mask_sptr.h

MANPAGE_SOURCES += rgrl_cast.h
MANPAGE_SOURCES += rgrl_set_of.txx

MANPAGE_SOURCES += rgrl_feature_sptr.h
SOURCES += rgrl_feature.cxx
SOURCES += rgrl_feature_point.cxx
SOURCES += rgrl_feature_trace_pt.cxx
SOURCES += rgrl_feature_face_pt.cxx
SOURCES += rgrl_feature_landmark.cxx

SOURCES += rgrl_feature_set.cxx
MANPAGE_SOURCES += rgrl_feature_set_sptr.h
MANPAGE_SOURCES += rgrl_feature_set_location.txx
#MANPAGE_SOURCES += rgrl_feature_set_signature.txx
MANPAGE_SOURCES += rgrl_feature_set_location_masked.txx

SOURCES += rgrl_match_set.cxx
MANPAGE_SOURCES += rgrl_match_set_sptr.h

SOURCES += rgrl_matcher.cxx
MANPAGE_SOURCES += rgrl_matcher_sptr.h
SOURCES += rgrl_matcher_k_nearest.cxx
SOURCES += rgrl_matcher_fixed.cxx
SOURCES += rgrl_matcher_k_nearest_boundary.cxx

SOURCES += rgrl_spline.cxx
MANPAGE_SOURCES += rgrl_spline_sptr.h

MANPAGE_SOURCES += rgrl_transformation_sptr.h
SOURCES += rgrl_transformation.cxx
SOURCES += rgrl_trans_translation.cxx
SOURCES += rgrl_trans_affine.cxx
SOURCES += rgrl_trans_rigid.cxx
SOURCES += rgrl_trans_similarity.cxx
SOURCES += rgrl_trans_spline.cxx
SOURCES += rgrl_trans_mixed_spline.cxx
SOURCES += rgrl_trans_quadratic.cxx
SOURCES += rgrl_trans_reduced_quad.cxx
SOURCES += rgrl_trans_reader.cxx
SOURCES += rgrl_trans_homography2d.cxx

SOURCES += rgrl_estimator.cxx
MANPAGE_SOURCES += rgrl_estimator_sptr.h
SOURCES += rgrl_est_translation.cxx
SOURCES += rgrl_est_affine.cxx
SOURCES += rgrl_est_rigid.cxx
SOURCES += rgrl_est_spline.cxx
SOURCES += rgrl_est_similarity2d.cxx
SOURCES += rgrl_est_quadratic.cxx
SOURCES += rgrl_est_reduced_quad2d.cxx
SOURCES += rgrl_est_homography2d.cxx
SOURCES += rgrl_est_homo2d_lm.cxx

SOURCES += rgrl_scale.cxx
MANPAGE_SOURCES += rgrl_scale_sptr.h
SOURCES += rgrl_view.cxx
SOURCES += rgrl_scale_estimator.cxx
MANPAGE_SOURCES += rgrl_scale_estimator_sptr.h
SOURCES += rgrl_scale_est_closest.cxx
SOURCES += rgrl_scale_est_all_weights.cxx
SOURCES += rgrl_scale_est_null.cxx

SOURCES += rgrl_weighter.cxx
MANPAGE_SOURCES += rgrl_weighter_sptr.h
SOURCES += rgrl_weighter_m_est.cxx
SOURCES += rgrl_weighter_unit.cxx

SOURCES += rgrl_data_manager.cxx
MANPAGE_SOURCES += rgrl_data_manager_sptr.h

SOURCES += rgrl_initializer.cxx
MANPAGE_SOURCES += rgrl_initializer_sptr.h
SOURCES += rgrl_initializer_prior.cxx
SOURCES += rgrl_initializer_ran_sam.cxx
SOURCES += rgrl_initializer_inv_indexing.cxx

SOURCES += rgrl_invariant.cxx
MANPAGE_SOURCES += rgrl_invariant_sptr.h
SOURCES += rgrl_invariant_match.cxx
MANPAGE_SOURCES += rgrl_invariant_match_sptr.h
SOURCES += rgrl_invariant_single_landmark.cxx

SOURCES += rgrl_convergence_tester.cxx
MANPAGE_SOURCES += rgrl_convergence_tester_sptr.h
SOURCES += rgrl_convergence_on_median_error.cxx
SOURCES += rgrl_convergence_on_weighted_error.cxx
SOURCES += rgrl_convergence_on_weighted_nas.cxx

SOURCES += rgrl_converge_status.cxx
MANPAGE_SOURCES += rgrl_converge_status_sptr.h
SOURCES += rgrl_converge_status_nas.cxx

SOURCES += rgrl_feature_based_registration.cxx

SOURCES += rgrl_debug_util.cxx
SOURCES += rgrl_util.cxx

MANPAGE_SOURCES += rgrl_event.h
MANPAGE_SOURCES += rgrl_event_sptr.h
MANPAGE_SOURCES += rgrl_command.h
MANPAGE_SOURCES += rgrl_command_sptr.h
MANPAGE_SOURCES += rgrl_view_sptr.h

MANPAGE_SOURCES += rgrl_macros.h
MANPAGE_SOURCES += rgrl_copyright.h

SOURCES += rgrl_evaluator_ssd.cxx
SOURCES += rgrl_feature_face_region.cxx
SOURCES += rgrl_feature_point_region.cxx
SOURCES += rgrl_feature_region.cxx
SOURCES += rgrl_feature_trace_region.cxx

MANPAGE_SOURCES += rgrl_evaluator.h
MANPAGE_SOURCES += rgrl_evaluator_sptr.h
MANPAGE_SOURCES += rgrl_matcher_pseudo.h
MANPAGE_SOURCES += rgrl_matcher_pseudo.txx
MANPAGE_SOURCES += rgrl_matcher_pseudo_3d.h
MANPAGE_SOURCES += rgrl_matcher_pseudo_3d.txx
MANPAGE_SOURCES += rgrl_matcher_pseudo_int_3d.h
MANPAGE_SOURCES += rgrl_matcher_pseudo_int_3d.txx
SOURCES += rgrl_feature_reader.cxx
SOURCES += rgrl_initializer_reader.cxx
SOURCES += rgrl_est_dis_homo2d_lm.cxx
SOURCES += rgrl_trans_rad_dis_homo2d.cxx
SOURCES += rgrl_trans_couple.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
