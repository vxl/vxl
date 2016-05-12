# bbas/bsta/CMakeLists.txt
# Brown statistics Library.

doxygen_add_library(contrib/brl/bbas/bsta
  DEPENDS core/vnl/algo core/vbl
  PACKAGE contrib/brl
  )

set(bsta_sources
 bsta_gauss.cxx            bsta_gauss.h
 bsta_histogram_base.h
 bsta_histogram.hxx        bsta_histogram.h       bsta_histogram_sptr.h     bsta_histogram.cxx
 bsta_joint_histogram_base.h
 bsta_joint_histogram.hxx  bsta_joint_histogram.h bsta_joint_histogram_sptr.h
 bsta_joint_histogram_3d.hxx      bsta_joint_histogram_3d.h
 bsta_joint_histogram_3d_base.h
 bsta_joint_histogram_3d_sptr.h
 bsta_k_medoid.cxx         bsta_k_medoid.h
 bsta_k_means.cxx         bsta_k_means.h    bsta_k_means.hxx
 bsta_otsu_threshold.hxx   bsta_otsu_threshold.h
 bsta_int_histogram_1d.cxx  bsta_int_histogram_1d.h
 bsta_int_histogram_2d.cxx  bsta_int_histogram_2d.h

  bsta_distribution.h

  bsta_gaussian.h
  bsta_gaussian_full.hxx         bsta_gaussian_full.h
  bsta_gaussian_indep.hxx        bsta_gaussian_indep.h
  bsta_gaussian_sphere.hxx       bsta_gaussian_sphere.h
  bsta_parzen_sphere.hxx         bsta_parzen_sphere.h
  bsta_parzen_sf1.h
  bsta_parzen_sf2.h
  bsta_parzen_sf3.h
  bsta_gauss_sf1.h               bsta_gauss_sd1.h
  bsta_gauss_sf2.h               bsta_gauss_sd2.h
  bsta_gauss_sf3.h               bsta_gauss_sd3.h
  bsta_gauss_if1.h               bsta_gauss_id1.h
  bsta_gauss_if2.h               bsta_gauss_id2.h
  bsta_gauss_if3.h               bsta_gauss_id3.h
  bsta_gauss_if4.h
  bsta_gauss_ff1.h               bsta_gauss_fd1.h
  bsta_gauss_ff2.h               bsta_gauss_fd2.h
  bsta_gauss_ff3.h               bsta_gauss_fd3.h

  bsta_mixture.h
  bsta_mixture_fixed.h
  bsta_basic_functors.h
  bsta_attributes.h
  bsta_detector_gaussian.h
  bsta_detector_mixture.h
  bsta_weibull.h   bsta_weibull.hxx
  bsta_sampler.h   bsta_sampler.hxx
  bsta_von_mises.h bsta_von_mises.hxx
  bsta_beta.h      bsta_beta.hxx
  bsta_beta_f1.h
  bsta_kent.h      bsta_kent.hxx
  bsta_spherical_histogram.h  bsta_spherical_histogram.hxx
  bsta_random_wrapper.h  bsta_random_wrapper.cxx
)

aux_source_directory(Templates bsta_sources)
vxl_add_library(LIBRARY_NAME bsta LIBRARY_SOURCES  ${bsta_sources})

target_link_libraries(bsta ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vnl_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()


# algorithms for statistical modeling
add_subdirectory(algo)

# Binary I/O clip on library
add_subdirectory(io)

# Visualization Tools for various bsta constructs
add_subdirectory(vis)

# process library
add_subdirectory(pro)
