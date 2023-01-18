#  core/vrel/CMakeLists.txt

doxygen_add_library(core/vrel
  DEPENDS core/vbl core/vgl core/vnl
  PACKAGE core/vrel
  DESCRIPTION "Robust Estimation Library"
  )

set(vrel_sources
 vrel_fwd.h
 vrel_mlesac_obj.cxx            vrel_mlesac_obj.h
 vrel_ransac_obj.cxx            vrel_ransac_obj.h
 vrel_lms_obj.cxx               vrel_lms_obj.h
 vrel_lts_obj.cxx               vrel_lts_obj.h
 vrel_muset_obj.cxx             vrel_muset_obj.h
 vrel_m_est_obj.cxx             vrel_m_est_obj.h
 vrel_tukey_obj.cxx             vrel_tukey_obj.h
 vrel_cauchy_obj.cxx            vrel_cauchy_obj.h
 vrel_trunc_quad_obj.cxx        vrel_trunc_quad_obj.h
 vrel_kernel_density_obj.cxx    vrel_kernel_density_obj.h

 vrel_misc.cxx                  vrel_misc.h
 vrel_muse_table.cxx            vrel_muse_table.h

 vrel_estimation_problem.cxx    vrel_estimation_problem.h
 vrel_linear_regression.cxx     vrel_linear_regression.h
 vrel_orthogonal_regression.cxx vrel_orthogonal_regression.h
 vrel_affine_est.cxx            vrel_affine_est.h
 vrel_quad_est.cxx              vrel_quad_est.h
 vrel_homography2d_est.cxx      vrel_homography2d_est.h
 vrel_homography2d_est_aff.cxx  vrel_homography2d_est_aff.h
 vrel_shift2d_est.cxx           vrel_shift2d_est.h

 vrel_irls.cxx                  vrel_irls.h
 vrel_ran_sam_search.cxx        vrel_ran_sam_search.h
 vrel_wgted_ran_sam_search.cxx  vrel_wgted_ran_sam_search.h

 vrel_util.hxx                  vrel_util.h

 vrel_objective.h
 vrel_wls_obj.h
)

aux_source_directory(Templates vrel_sources)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

vxl_add_library(LIBRARY_NAME vrel LIBRARY_SOURCES ${vrel_sources})
target_link_libraries(vrel ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl)

set(CURR_LIB_NAME vrel)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     USE_HIDDEN_VISIBILITY
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
