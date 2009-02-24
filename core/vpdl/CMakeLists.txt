# core/vpdl/CMakeLists.txt
# Vision Probability Distribution Library

SET( vpdl_sources
  vpdl_npower.h            vpdl_npower.cxx
  vpdl_eigen_sym_matrix.h
  vpdl_update_gaussian.h
  
  vpdl_base_traits.h
  vpdl_distribution.h      vpdl_distribution.txx
  
  vpdl_gaussian_base.h
  vpdl_gaussian_sphere.h
  vpdl_gaussian_indep.h
  vpdl_gaussian.h
  
  vpdl_multi_cmp_dist.h
  vpdl_mixture.h
  vpdl_kernel_base.h
  vpdl_kernel_gaussian_sfbw.h
)

AUX_SOURCE_DIRECTORY(Templates vpdl_sources)

ADD_LIBRARY(vpdl ${vpdl_sources})
TARGET_LINK_LIBRARIES( vpdl vnl_algo )

INSTALL_TARGETS(/lib vpdl)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vpdl ${vpdl_sources})


IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
