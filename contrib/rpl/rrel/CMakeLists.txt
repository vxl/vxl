# rpl/rrel/CMakeLists.txt

doxygen_add_library(contrib/rpl/rrel
  DEPENDS core/vbl core/vgl core/vnl
  PACKAGE contrib/rpl
  DESCRIPTION "Robust Estimation Library"
  )

set(rrel_sources
 rrel_export.h                  rrel_fwd.h
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

 rrel_estimation_problem.cxx    rrel_estimation_problem.h
 rrel_linear_regression.cxx     rrel_linear_regression.h
 rrel_orthogonal_regression.cxx rrel_orthogonal_regression.h
 rrel_affine_est.cxx            rrel_affine_est.h
 rrel_quad_est.cxx              rrel_quad_est.h
 rrel_homography2d_est.cxx      rrel_homography2d_est.h
 rrel_homography2d_est_aff.cxx  rrel_homography2d_est_aff.h
 rrel_shift2d_est.cxx           rrel_shift2d_est.h

 rrel_irls.cxx                  rrel_irls.h
 rrel_ran_sam_search.cxx        rrel_ran_sam_search.h
 rrel_wgted_ran_sam_search.cxx  rrel_wgted_ran_sam_search.h

 rrel_util.hxx                  rrel_util.h

 rrel_objective.h
 rrel_wls_obj.h
)

aux_source_directory(Templates rrel_sources)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

vxl_add_library(LIBRARY_NAME rrel LIBRARY_SOURCES ${rrel_sources})
target_link_libraries(rrel ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl)

set(CURR_LIB_NAME rrel)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${RPL_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR ${RPL_INSTALL_INCLUDE_DIR}/${CURR_LIB_NAME}
     USE_HIDDEN_VISIBILITY
)

include_directories(${MUL_INCLUDE_DIR})
if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
