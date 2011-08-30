ADD_EXECUTABLE( bundler_test_all test_driver.cxx

  test_bundler_utils.cxx
  test_tracks_detect.cxx
  test_propose_matches.cxx
  test_match_ann.cxx )


TARGET_LINK_LIBRARIES(bundler_test_all bundler vpgl_algo vpgl vpgl_file_formats vsol bgeo vgl_algo vnl_algo vnl vgl vil vul vbl rrel bapl rsdl testlib)

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

ADD_EXECUTABLE( bundler_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bundler_test_include bundler )
