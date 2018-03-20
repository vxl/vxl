# core/vpdl/CMakeLists.txt
# Vision Probability Distribution Library

doxygen_add_library(core/vpdl
  DEPENDS core/vnl/algo core/vnl
  PACKAGE core-L2E
  DESCRIPTION "Probability Distribution Library"
  )

set( vpdl_sources
  vpdl_distribution.h      vpdl_distribution.hxx

  vpdl_gaussian_base.h
  vpdl_gaussian_sphere.h
  vpdl_gaussian_indep.h
  vpdl_gaussian.h

  vpdl_multi_cmp_dist.h
  vpdl_mixture.h
  vpdl_mixture_of.h
  vpdl_kernel_base.h
  vpdl_kernel_gaussian_sfbw.h

  # Template Library headers
  vpdt/vpdt_access.h
  vpdt/vpdt_enable_if.h
  vpdt/vpdt_field_traits.h
  vpdt/vpdt_field_default.h
  vpdt/vpdt_dist_traits.h
  vpdt/vpdt_probability.h
  vpdt/vpdt_log_probability.h
  vpdt/vpdt_num_obs.h

  vpdt/vpdt_norm_metric.h
  vpdt/vpdt_eigen_sym_matrix.h
  vpdt/vpdt_gaussian.h
  vpdt/vpdt_mixture_of.h
  vpdt/vpdt_update_gaussian.h
  vpdt/vpdt_update_mog.h
  vpdt/vpdt_mog_fitness.h

  vpdt/vpdt_distribution_accessors.h
  vpdt/vpdt_mixture_accessors.h
  vpdt/vpdt_gaussian_detector.h
  vpdt/vpdt_mixture_detector.h
)

aux_source_directory(Templates vpdl_sources)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vpdl LIBRARY_SOURCES ${vpdl_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vpdl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl )
set(CURR_LIB_NAME vpdl)
set_vxl_library_properties(
     TARGET_NAME ${VXL_LIB_PREFIX}${CURR_LIB_NAME}
     BASE_NAME ${CURR_LIB_NAME}
     EXPORT_HEADER_FILE ${VXLCORE_BINARY_INCLUDE_DIR}/${CURR_LIB_NAME}/${CURR_LIB_NAME}_export.h
     INSTALL_DIR   ${VXL_INSTALL_INCLUDE_DIR}/core/${CURR_LIB_NAME}
     #USE_HIDDEN_VISIBILITY
)
if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
