# brl/bbas/bsta/algo/tests/CMakeLists.txt
# Tests for bsta/algo library

add_executable( bsta_algo_test_all
  test_driver.cxx
  test_fit_weibull.cxx
  test_gaussian_model.cxx
  test_mean_shift.cxx
  test_fit_gaussian.cxx
  test_von_mises_update.cxx
  test_beta_updater.cxx
  test_rand_sampling.cxx
  test_display_vrml.cxx
)

target_link_libraries( bsta_algo_test_all bsta_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_100.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_10_100.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_10_100.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beta_distr_100_10.txt ${CMAKE_CURRENT_BINARY_DIR}/beta_distr_100_10.txt COPYONLY )

add_test( NAME bsta_algo_test_fit_weibull COMMAND $<TARGET_FILE:bsta_algo_test_all> test_fit_weibull )
add_test( NAME bsta_algo_test_gaussian_model COMMAND $<TARGET_FILE:bsta_algo_test_all> test_gaussian_model )
if( VXL_RUN_FAILING_TESTS )
add_test( NAME bsta_algo_test_mean_shift COMMAND $<TARGET_FILE:bsta_algo_test_all> test_mean_shift )
endif()
add_test( NAME bsta_algo_test_fit_gaussian COMMAND $<TARGET_FILE:bsta_algo_test_all> test_fit_gaussian )
add_test( NAME bsta_algo_test_von_mises_update COMMAND $<TARGET_FILE:bsta_algo_test_all> test_von_mises_update )
add_test( NAME bsta_algo_test_beta_updater COMMAND $<TARGET_FILE:bsta_algo_test_all> test_beta_updater )
# add_test( NAME bsta_algo_test_census_updater COMMAND $<TARGET_FILE:bsta_algo_test_all> test_census_updater)
add_test( NAME bsta_algo_test_rand_sampling COMMAND $<TARGET_FILE:bsta_algo_test_all> test_rand_sampling )
add_test( NAME bsta_algo_test_display_vrml COMMAND $<TARGET_FILE:bsta_algo_test_all> test_display_vrml )
add_executable( bsta_algo_test_include test_include.cxx )
target_link_libraries( bsta_algo_test_include bsta_algo )
add_executable( bsta_algo_test_template_include test_template_include.cxx )
target_link_libraries( bsta_algo_test_template_include bsta_algo )
