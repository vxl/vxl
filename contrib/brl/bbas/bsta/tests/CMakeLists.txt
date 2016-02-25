# brl/bbas/bsta/tests/CMakeLists.txt
# Tests for bsta library

add_executable( bsta_test_all
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

target_link_libraries( bsta_test_all bsta bsta_io ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_2_2.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_2_2.txt COPYONLY )

add_test( NAME bsta_test_gaussian_full COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_full )
add_test( NAME bsta_test_gaussian_indep COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_indep )
add_test( NAME bsta_test_gaussian_sphere COMMAND $<TARGET_FILE:bsta_test_all> test_gaussian_sphere )
add_test( NAME bsta_test_parzen_sphere COMMAND $<TARGET_FILE:bsta_test_all> test_parzen_sphere )
add_test( NAME bsta_test_weibull COMMAND $<TARGET_FILE:bsta_test_all> test_weibull )
add_test( NAME bsta_test_von_mises COMMAND $<TARGET_FILE:bsta_test_all> test_von_mises )
add_test( NAME bsta_test_mixture COMMAND $<TARGET_FILE:bsta_test_all> test_mixture )
add_test( NAME bsta_test_histogram COMMAND $<TARGET_FILE:bsta_test_all> test_bsta_histogram )
add_test( NAME bsta_test_k_medoid COMMAND $<TARGET_FILE:bsta_test_all> test_k_medoid )
add_test( NAME bsta_test_k_means COMMAND $<TARGET_FILE:bsta_test_all> test_k_means )
add_test( NAME bsta_test_otsu_threshold COMMAND $<TARGET_FILE:bsta_test_all> test_otsu_threshold )
add_test( NAME bsta_test_sampler COMMAND $<TARGET_FILE:bsta_test_all> test_sampler )
add_test( NAME bsta_test_beta COMMAND $<TARGET_FILE:bsta_test_all> test_beta )
add_test( NAME bsta_test_kent COMMAND $<TARGET_FILE:bsta_test_all> test_kent )
add_test( NAME bsta_test_spherical_histogram COMMAND $<TARGET_FILE:bsta_test_all> test_spherical_histogram )

add_executable( bsta_test_include test_include.cxx )
target_link_libraries( bsta_test_include bsta ${VXL_LIB_PREFIX}vsl)
add_executable( bsta_test_template_include test_template_include.cxx )
target_link_libraries( bsta_test_template_include bsta ${VXL_LIB_PREFIX}vsl)
