IF(vxl_SOURCE_DIR)
  SET(VXL_MUL_INCLUDE_DIR ${MUL_INCLUDE_DIR})
ENDIF(vxl_SOURCE_DIR)

INCLUDE_DIRECTORIES( ${VXL_MUL_INCLUDE_DIR} )

SET( rgrl_sources
  rgrl_fwd.h
  rgrl_object.h                        rgrl_object.cxx
  rgrl_object_sptr.h

  rgrl_mask.h                          rgrl_mask.cxx                 
  rgrl_mask_sptr.h

  rgrl_cast.h
  rgrl_set_of.h                        rgrl_set_of.txx

  rgrl_feature.h                       rgrl_feature.cxx              
  rgrl_feature_sptr.h
  rgrl_feature_point.h                 rgrl_feature_point.cxx
  rgrl_feature_trace_pt.h              rgrl_feature_trace_pt.cxx
  rgrl_feature_face_pt.h               rgrl_feature_face_pt.cxx
  rgrl_feature_landmark.h              rgrl_feature_landmark.cxx

  rgrl_feature_set.h                   rgrl_feature_set.cxx          
  rgrl_feature_set_sptr.h
  rgrl_feature_set_location.h          rgrl_feature_set_location.txx
  rgrl_feature_set_location_masked.h   rgrl_feature_set_location_masked.txx

  rgrl_match_set.h                     rgrl_match_set.cxx            
  rgrl_match_set_sptr.h

  rgrl_matcher.h                       rgrl_matcher.cxx              
  rgrl_matcher_sptr.h
  rgrl_matcher_k_nearest.h             rgrl_matcher_k_nearest.cxx
  rgrl_matcher_fixed.h                 rgrl_matcher_fixed.cxx
  rgrl_matcher_k_nearest_boundary.h    rgrl_matcher_k_nearest_boundary.cxx

  rgrl_spline.h                        rgrl_spline.cxx               
  rgrl_spline_sptr.h

  rgrl_transformation.h                rgrl_transformation.cxx       
  rgrl_transformation_sptr.h
  rgrl_trans_translation.h             rgrl_trans_translation.cxx
  rgrl_trans_affine.h                  rgrl_trans_affine.cxx
  rgrl_trans_rigid.h                   rgrl_trans_rigid.cxx
  rgrl_trans_similarity.h              rgrl_trans_similarity.cxx
  rgrl_trans_spline.h                  rgrl_trans_spline.cxx
  rgrl_trans_mixed_spline.h            rgrl_trans_mixed_spline.cxx
  rgrl_trans_quadratic.h               rgrl_trans_quadratic.cxx
  rgrl_trans_reduced_quad.h            rgrl_trans_reduced_quad.cxx
  rgrl_trans_homography2d.h            rgrl_trans_homography2d.cxx
  rgrl_trans_reader.h                  rgrl_trans_reader.cxx

  rgrl_estimator.h                     rgrl_estimator.cxx            
  rgrl_estimator_sptr.h
  rgrl_est_translation.h               rgrl_est_translation.cxx
  rgrl_est_affine.h                    rgrl_est_affine.cxx
  rgrl_est_rigid.h                     rgrl_est_rigid.cxx
  rgrl_est_spline.h                    rgrl_est_spline.cxx
  rgrl_est_similarity2d.h              rgrl_est_similarity2d.cxx
  rgrl_est_quadratic.h                 rgrl_est_quadratic.cxx
  rgrl_est_reduced_quad2d.h            rgrl_est_reduced_quad2d.cxx
  rgrl_est_homography2d.h              rgrl_est_homography2d.cxx
  rgrl_est_homo2d_lm.h                rgrl_est_homo2d_lm.cxx
                                     
  rgrl_scale.h                         rgrl_scale.cxx                
  rgrl_scale_sptr.h
  rgrl_view.h                          rgrl_view.cxx
  rgrl_scale_estimator.h               rgrl_scale_estimator.cxx      
  rgrl_scale_estimator_sptr.h
  rgrl_scale_est_closest.h             rgrl_scale_est_closest.cxx
  rgrl_scale_est_all_weights.h         rgrl_scale_est_all_weights.cxx
  rgrl_scale_est_null.h                rgrl_scale_est_null.cxx

  rgrl_weighter.h                      rgrl_weighter.cxx             
  rgrl_weighter_sptr.h
  rgrl_weighter_m_est.h                rgrl_weighter_m_est.cxx
  rgrl_weighter_unit.h                 rgrl_weighter_unit.cxx

  rgrl_data_manager.h                  rgrl_data_manager.cxx         
  rgrl_data_manager_sptr.h

  rgrl_initializer.h                   rgrl_initializer.cxx          
  rgrl_initializer_sptr.h
  rgrl_initializer_prior.h             rgrl_initializer_prior.cxx
  rgrl_initializer_ran_sam.h           rgrl_initializer_ran_sam.cxx
  rgrl_initializer_inv_indexing.h      rgrl_initializer_inv_indexing.cxx

  rgrl_invariant.h                     rgrl_invariant.cxx            
  rgrl_invariant_sptr.h
  rgrl_invariant_match.h               rgrl_invariant_match.cxx      
  rgrl_invariant_match_sptr.h
  rgrl_invariant_single_landmark.h     rgrl_invariant_single_landmark.cxx

  rgrl_convergence_tester.h            rgrl_convergence_tester.cxx   
  rgrl_convergence_tester_sptr.h
  rgrl_convergence_on_median_error.h   rgrl_convergence_on_median_error.cxx
  rgrl_convergence_on_weighted_error.h rgrl_convergence_on_weighted_error.cxx

  rgrl_converge_status.h               rgrl_converge_status.cxx      
  rgrl_converge_status_sptr.h

  rgrl_feature_based_registration.h    rgrl_feature_based_registration.cxx

  rgrl_debug_util.h                    rgrl_debug_util.cxx
  rgrl_util.h                          rgrl_util.cxx

  rgrl_event.h                         rgrl_event_sptr.h
  rgrl_command.h                       rgrl_command_sptr.h
                                       rgrl_view_sptr.h

  rgrl_macros.h
  rgrl_copyright.h
)

AUX_SOURCE_DIRECTORY( Templates rgrl_sources )

ADD_LIBRARY( rgrl ${rgrl_sources} )
TARGET_LINK_LIBRARIES( rgrl rrel rsdl vil3d vil vnl_algo vnl vbl vul)
#FIND_PACKAGE( ITK )
#IF( ITK_FOUND )
#    TARGET_LINK_LIBRARIES( rgrl ITKCommon )
#ENDIF( ITK_FOUND )

IF( BUILD_TESTING )
  SUBDIRS( tests )
ENDIF( BUILD_TESTING )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )
