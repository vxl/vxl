# core/vpdl/CMakeLists.txt
# Vision Probability Distribution Library

doxygen_add_library(core/vpdl
  DEPENDS core/vnl/algo core/vnl
  PACKAGE core-L2E
  DESCRIPTION "Probability Distribution Library"
  )

SET( vpdl_sources
  vpdl_distribution.h      vpdl_distribution.txx

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

AUX_SOURCE_DIRECTORY(Templates vpdl_sources)

ADD_LIBRARY(vpdl ${vpdl_sources})
TARGET_LINK_LIBRARIES( vpdl vnl_algo vnl )

INSTALL_TARGETS(/lib vpdl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpdl ${vpdl_sources})

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
