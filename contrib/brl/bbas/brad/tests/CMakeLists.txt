SET( brad_test_sources
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
   )

ADD_EXECUTABLE(brad_test_all ${brad_test_sources})
TARGET_LINK_LIBRARIES( brad_test_all brad bsta_io vnl_algo vnl vil testlib)

ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_illum)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_pos)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_hist)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_sun_dir_index)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_phongs_model_est)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_feature_pca)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_synoptic_function)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_image_metadata)
ADD_TEST( brad_test_all ${EXECUTABLE_OUTPUT_PATH}/brad_test_all test_atmospheric_parameters)

ADD_EXECUTABLE( brad_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brad_test_include brad)
ADD_EXECUTABLE( brad_test_template_include test_template_include.cxx )
TARGET_LINK_LIBRARIES( brad_test_template_include brad )

