#ifndef rgrl_fwd_h_
#define rgrl_fwd_h_
//:
// \file
// \brief for forward delarations
// \date 04 Aug 2004

class rgrl_command;
class rgrl_converge_status;
class rgrl_convergence_on_median_error;
class rgrl_convergence_on_weighted_error;
class rgrl_convergence_tester;
class rgrl_data_manager;
class rgrl_est_affine;
class rgrl_est_quadratic;
class rgrl_est_reduced_quad2d;
class rgrl_est_rigid;
class rgrl_est_similarity2d;
class rgrl_est_spline;
class rgrl_est_translation;
class rgrl_estimator;
class rgrl_event;
class rgrl_feature;
class rgrl_feature_based_registration;
class rgrl_feature_face_pt;
class rgrl_feature_face_region;
class rgrl_feature_landmark;
class rgrl_feature_point;
class rgrl_feature_set;
class rgrl_feature_set_location_kd_tree;
class rgrl_feature_trace_pt;
class rgrl_feature_trace_region;
class rgrl_initializer;
class rgrl_initializer_inv_indexing;
class rgrl_initializer_prior;
class rgrl_initializer_ran_sam;
class rgrl_invariant;
class rgrl_invariant_match;
class rgrl_invariant_single_landmark;
class rgrl_mask;
class rgrl_mask_2d_image;
class rgrl_mask_3d_image;
class rgrl_mask_sphere;
class rgrl_mask_box;
class rgrl_match_set;
class rgrl_matcher;
class rgrl_matcher_fixed;
class rgrl_matcher_k_nearest;
class rgrl_matcher_k_nearest_boundary;
class rgrl_object;
class rgrl_scale;
class rgrl_scale_est_all_weights;
class rgrl_scale_est_closest;
class rgrl_scale_est_null;
class rgrl_scale_estimator;
class rgrl_spline;
class rgrl_trans_affine;
class rgrl_trans_quadratic;
class rgrl_trans_reader;
class rgrl_trans_reduced_quad;
class rgrl_trans_rigid;
class rgrl_trans_similarity;
class rgrl_trans_spline;
class rgrl_trans_translation;
class rgrl_transformation;
class rgrl_weighter;
class rgrl_weighter_m_est;
class rgrl_weighter_unit;

template<unsigned N> class rgrl_feature_set_location;
template<unsigned N> class rgrl_feature_set_location_masked;

#endif
