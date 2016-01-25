# brl/bbas/bsta/tests/CMakeLists.txt
# Tests for bsta library

ADD_EXECUTABLE( bsta_test_all
  test_driver.cxx
  test_gaussian_full.cxx
  test_gaussian_indep.cxx
  test_gaussian_sphere.cxx
  test_mixture.cxx
  test_bsta_histogram.cxx
  test_k_medoid.cxx
  test_k_means.cxx
  test_otsu_threshold.cxx
  test_parzen_sphere.cxx
  test_weibull.cxx
  test_sampler.cxx
  test_von_mises.cxx
  test_beta.cxx
  test_kent.cxx
  test_spherical_histogram.cxx
)

TARGET_LINK_LIBRARIES( bsta_test_all bsta bsta_io vnl vpl vbl_io vsl vbl vgl testlib )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_2_2.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_2_2.txt COPYONLY )

ADD_TEST( NAME bsta_test_gaussian_full COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_full )
ADD_TEST( NAME bsta_test_gaussian_indep COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_indep )
ADD_TEST( NAME bsta_test_gaussian_sphere COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_sphere )
ADD_TEST( NAME bsta_test_parzen_sphere COMMAND $<TARGET_FILE:bsta_test_all> test_parzen_sphere )
ADD_TEST( NAME bsta_test_weibull COMMAND $<TARGET_FILE:bsta_test_all> test_weibull )
ADD_TEST( NAME bsta_test_von_mises COMMAND $<TARGET_FILE:bsta_test_all> test_von_mises )
ADD_TEST( NAME bsta_test_mixture COMMAND $<TARGET_FILE:bsta_test_all> test_mixture )
ADD_TEST( NAME bsta_test_histogram COMMAND $<TARGET_FILE:bsta_test_all> test_bsta_histogram )
ADD_TEST( NAME bsta_test_k_medoid COMMAND $<TARGET_FILE:bsta_test_all> test_k_medoid )
ADD_TEST( NAME bsta_test_k_means COMMAND $<TARGET_FILE:bsta_test_all> test_k_means )
ADD_TEST( NAME bsta_test_otsu_threshold COMMAND $<TARGET_FILE:bsta_test_all> test_otsu_threshold )
ADD_TEST( NAME bsta_test_sampler COMMAND $<TARGET_FILE:bsta_test_all> test_sampler )
ADD_TEST( NAME bsta_test_beta COMMAND $<TARGET_FILE:bsta_test_all> test_beta )
ADD_TEST( NAME bsta_test_kent COMMAND $<TARGET_FILE:bsta_test_all> test_kent )
ADD_TEST( NAME bsta_test_spherical_histogram COMMAND $<TARGET_FILE:bsta_test_all> test_spherical_histogram )

ADD_EXECUTABLE( bsta_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bsta_test_include bsta vsl)
ADD_EXECUTABLE( bsta_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( bsta_test_template_include bsta vsl)
