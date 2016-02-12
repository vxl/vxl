# mul/mfpf/tests/CMakeLists.txt

add_executable( mfpf_test_all
  test_driver.cxx
  test_edge_finder.cxx
  test_norm_corr1d.cxx
  test_norm_corr2d.cxx
  test_profile_pdf.cxx
  test_region_pdf.cxx
  test_region_finder.cxx
  test_pose.cxx
  test_mr_point_finder.cxx
  test_dp_snake.cxx
  test_patch_data.cxx
  test_pose_predictor.cxx
  )

target_link_libraries( mfpf_test_all mfpf ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}testlib )

add_test( NAME mfpf_test_edge_finder COMMAND $<TARGET_FILE:mfpf_test_all> test_edge_finder)
add_test( NAME mfpf_test_norm_corr1d COMMAND $<TARGET_FILE:mfpf_test_all> test_norm_corr1d)
add_test( NAME mfpf_test_norm_corr2d COMMAND $<TARGET_FILE:mfpf_test_all> test_norm_corr2d)
add_test( NAME mfpf_test_profile_pdf COMMAND $<TARGET_FILE:mfpf_test_all> test_profile_pdf)
add_test( NAME mfpf_test_region_pdf COMMAND $<TARGET_FILE:mfpf_test_all> test_region_pdf)
add_test( NAME mfpf_test_region_finder COMMAND $<TARGET_FILE:mfpf_test_all> test_region_finder)
add_test( NAME mfpf_test_pose COMMAND $<TARGET_FILE:mfpf_test_all> test_pose)
add_test( NAME mfpf_test_mr_point_finder COMMAND $<TARGET_FILE:mfpf_test_all> test_mr_point_finder)
add_test( NAME mfpf_test_patch_data COMMAND $<TARGET_FILE:mfpf_test_all> test_patch_data)
add_test( NAME mfpf_test_dp_snake COMMAND $<TARGET_FILE:mfpf_test_all> test_dp_snake)
add_test( NAME mfpf_test_pose_predictor COMMAND $<TARGET_FILE:mfpf_test_all> test_pose_predictor)

add_executable( mfpf_test_include test_include.cxx )
target_link_libraries( mfpf_test_include mfpf mipa)
