ADD_LIBRARY( rrel rrel_sources )

SOURCE_FILES( rrel_sources
  rrel_mlesac_obj
  rrel_ransac_obj
  rrel_lms_obj
  rrel_lts_obj
  rrel_muset_obj
  rrel_m_est_obj
  rrel_tukey_obj
  rrel_cauchy_obj
  rrel_trunc_quad_obj

  rrel_misc
  rrel_muse_table

  rrel_estimation_problem
  rrel_linear_regression
  rrel_orthogonal_regression
  rrel_homography2d_est

  rrel_irls
  rrel_ran_sam_search
)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/mul )

AUX_SOURCE_DIRECTORY( Templates rrel_sources )

#SUBDIRS(examples)

IF( BUILD_TESTS)
  SUBDIRS(tests)
ENDIF( BUILD_TESTS)
