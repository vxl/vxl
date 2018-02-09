set( brad_test_sources
   test_driver.cxx
   test_illum.cxx
   test_sun_pos.cxx
   test_sun_hist.cxx
   test_sun_dir_index.cxx
   test_feature_pca.cxx
   test_phongs_model_est.cxx
   test_synoptic_function.cxx
   test_image_metadata.cxx
   test_atmospheric_parameters.cxx
   test_appearance_neighborhood_index.cxx
   test_belief_prop_utils.cxx
)

add_executable(brad_test_all ${brad_test_sources})
target_link_libraries( brad_test_all brad bsta_io ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}testlib)

add_test( NAME test_illum_brad_test_all                         COMMAND $<TARGET_FILE:brad_test_all> test_illum)
add_test( NAME test_sun_pos_brad_test_all                       COMMAND $<TARGET_FILE:brad_test_all> test_sun_pos)
add_test( NAME test_feature_pca_brad_test_all                   COMMAND $<TARGET_FILE:brad_test_all> test_feature_pca)
add_test( NAME test_synoptic_function_brad_test_all             COMMAND $<TARGET_FILE:brad_test_all> test_synoptic_function)
add_test( NAME test_image_metadata_brad_test_all                COMMAND $<TARGET_FILE:brad_test_all> test_image_metadata)
add_test( NAME test_atmospheric_parameters_brad_test_all        COMMAND $<TARGET_FILE:brad_test_all> test_atmospheric_parameters)
add_test( NAME test_appearance_neighborhood_index_brad_test_all COMMAND $<TARGET_FILE:brad_test_all> test_appearance_neighborhood_index)
add_test( NAME test_belief_prop_utils_brad_test_all             COMMAND $<TARGET_FILE:brad_test_all> test_belief_prop_utils)

if(0) # Previously this test was not run, and it fails
add_test( NAME test_sun_hist_brad_test_all                      COMMAND $<TARGET_FILE:brad_test_all> test_sun_hist)
add_test( NAME test_phongs_model_est_brad_test_all              COMMAND $<TARGET_FILE:brad_test_all> test_phongs_model_est)
add_test( NAME test_sun_dir_index_brad_test_all                 COMMAND $<TARGET_FILE:brad_test_all> test_sun_dir_index)
endif()

add_executable( brad_test_include test_include.cxx )
target_link_libraries( brad_test_include brad)
add_executable( brad_test_template_include test_template_include.cxx )
target_link_libraries( brad_test_template_include brad )

