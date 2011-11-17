ADD_EXECUTABLE( bundler_test_all test_driver.cxx

  test_bundler_utils.cxx
  test_tracks_detect.cxx
  test_propose_matches.cxx
  test_match_ann.cxx
  test_refine.cxx 
  test_tracks.cxx
  test_initial_recon.cxx
  test_pipeline.cxx
  test_ply.cxx
  test_add_next_image.cxx 
  test_bundle_adjust.cxx
  utils.cxx     utils.h)


TARGET_LINK_LIBRARIES(bundler_test_all bundler vpgl_algo vpgl vpgl_file_formats vsol vgl_algo vnl_algo vnl vgl vil vul vbl rrel bapl rsdl testlib)

ADD_TEST( 
    bundler_test_utils 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_bundler_utils )

ADD_TEST( 
    bundler_test_tracks_detect 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_tracks_detect 
    ${cul_SOURCE_DIR}/bundler/tests/test_data/checkers.png)

ADD_TEST( 
    bundler_test_propose_matches 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_propose_matches )

ADD_TEST( 
    bundler_test_match_ann 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_match_ann )

ADD_TEST( 
    bundler_test_refine 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_refine )

ADD_TEST( 
    bundler_test_tracks 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_tracks
    ${cul_SOURCE_DIR}/bundler/tests/test_data)

ADD_TEST( 
    bundler_test_initial_recon 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_initial_recon )

ADD_TEST( 
    bundler_test_pipeline 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_pipeline )

ADD_TEST( 
    bundler_test_ply 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_ply )

ADD_TEST( 
    bundler_test_add_next_image 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_add_next_image )

ADD_TEST( 
    bundler_test_bundle_adjust 
    ${EXECUTABLE_OUTPUT_PATH}/bundler_test_all 
    test_bundle_adjust )


ADD_EXECUTABLE( bundler_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bundler_test_include bundler )
