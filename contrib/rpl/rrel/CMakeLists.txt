# rpl/rrel/CMakeLists.txt

SET(rrel_sources
 rrel_mlesac_obj.cxx            rrel_mlesac_obj.h
 rrel_ransac_obj.cxx            rrel_ransac_obj.h
 rrel_lms_obj.cxx               rrel_lms_obj.h
 rrel_lts_obj.cxx               rrel_lts_obj.h
 rrel_muset_obj.cxx             rrel_muset_obj.h
 rrel_m_est_obj.cxx             rrel_m_est_obj.h
 rrel_tukey_obj.cxx             rrel_tukey_obj.h
 rrel_cauchy_obj.cxx            rrel_cauchy_obj.h
 rrel_trunc_quad_obj.cxx        rrel_trunc_quad_obj.h
 rrel_kernel_density_obj.cxx    rrel_kernel_density_obj.h

 rrel_misc.cxx                  rrel_misc.h
 rrel_muse_table.cxx            rrel_muse_table.h
 rrel_muse_table_old.cxx        rrel_muse_table_old.h

 rrel_estimation_problem.cxx    rrel_estimation_problem.h
 rrel_linear_regression.cxx     rrel_linear_regression.h
 rrel_orthogonal_regression.cxx rrel_orthogonal_regression.h
 rrel_homography2d_est.cxx      rrel_homography2d_est.h
 rrel_homography2d_est_aff.cxx  rrel_homography2d_est_aff.h
 rrel_shift2d_est.cxx           rrel_shift2d_est.h

 rrel_irls.cxx                  rrel_irls.h
 rrel_ran_sam_search.cxx        rrel_ran_sam_search.h

 rrel_util.txx                  rrel_util.h

 rrel_objective.h
 rrel_wls_obj.h
)

AUX_SOURCE_DIRECTORY(Templates rrel_sources)

ADD_LIBRARY(rrel ${rrel_sources})
TARGET_LINK_LIBRARIES(rrel vnl_algo vnl vgl vbl)

INCLUDE_DIRECTORIES(${VXLCORE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${MUL_INCLUDE_DIR})

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
