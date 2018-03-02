add_executable( bundler_test_all test_driver.cxx

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


target_link_libraries(bundler_test_all bundler ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_file_formats vsol ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl rrel bapl rsdl ${VXL_LIB_PREFIX}testlib)

add_test( NAME bundler_test_utils
          COMMAND $<TARGET_FILE:bundler_test_all> test_bundler_utils )
add_test( NAME bundler_test_tracks_detect
          COMMAND $<TARGET_FILE:bundler_test_all> test_tracks_detect ${cul_SOURCE_DIR}/bundler/tests/test_data/checkers.png)
add_test( NAME bundler_test_propose_matches
          COMMAND $<TARGET_FILE:bundler_test_all> test_propose_matches )
add_test( NAME bundler_test_tracks
          COMMAND $<TARGET_FILE:bundler_test_all> test_tracks ${cul_SOURCE_DIR}/bundler/tests/test_data)
add_test( NAME bundler_test_ply
          COMMAND $<TARGET_FILE:bundler_test_all> test_ply )
add_test( NAME bundler_test_bundle_adjust
          COMMAND $<TARGET_FILE:bundler_test_all> test_bundle_adjust )

if(VXL_RUN_FAILING_TESTS)
add_test( NAME bundler_test_match_ann
          COMMAND $<TARGET_FILE:bundler_test_all> test_match_ann )
add_test( NAME bundler_test_refine
          COMMAND $<TARGET_FILE:bundler_test_all> test_refine )
add_test( NAME bundler_test_initial_recon
          COMMAND $<TARGET_FILE:bundler_test_all> test_initial_recon )
add_test( NAME bundler_test_pipeline
          COMMAND $<TARGET_FILE:bundler_test_all> test_pipeline )
add_test( NAME bundler_test_add_next_image
          COMMAND $<TARGET_FILE:bundler_test_all> test_add_next_image )
endif()


add_executable( bundler_test_include test_include.cxx )
target_link_libraries( bundler_test_include bundler )
