# ./rpl/rrel/CMakeLists.txt

ADD_LIBRARY( rrel rrel_sources )

SOURCE_FILES( rrel_sources
  rrel_mlesac_obj.cxx
  rrel_ransac_obj.cxx
  rrel_lms_obj.cxx
  rrel_lts_obj.cxx
  rrel_muset_obj.cxx
  rrel_m_est_obj.cxx
  rrel_tukey_obj.cxx
  rrel_cauchy_obj.cxx
  rrel_trunc_quad_obj.cxx

  rrel_misc.cxx
  rrel_muse_table.cxx

  rrel_estimation_problem.cxx
  rrel_linear_regression.cxx
  rrel_orthogonal_regression.cxx
  rrel_homography2d_est.cxx

  rrel_irls.cxx
  rrel_ran_sam_search.cxx
)

AUX_SOURCE_DIRECTORY( Templates rrel_sources )

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/rpl/rrel/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/vxl )
INCLUDE_DIRECTORIES( ${allvxl_SOURCE_DIR}/mul )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
