# brl/bseg/brip/tests/CMakeLists.txt

add_executable( brip_test_all
  test_driver.cxx
  test_histogram.cxx
  test_mutual_info.cxx
  test_watershed.cxx
  test_fourier.cxx
  test_homography.cxx
  test_lucas_kanade.cxx
  test_Horn_Schunck.cxx
  test_quadtree.cxx
  test_max_scale_response.cxx
  test_mask.cxx
  test_label_equivalence.cxx
  test_extrema.cxx
  test_filter_bank.cxx
  test_gain_offset_solver.cxx
  test_nitf_ops.cxx
  test_phase_correlation.cxx
)
target_link_libraries( brip_test_all brip ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}testlib)

add_test( NAME brip_test_histogram COMMAND $<TARGET_FILE:brip_test_all> test_histogram )
add_test( NAME brip_test_mutual_info COMMAND $<TARGET_FILE:brip_test_all> test_mutual_info )
add_test( NAME brip_test_watershed COMMAND $<TARGET_FILE:brip_test_all> test_watershed )
add_test( NAME brip_test_fourier COMMAND $<TARGET_FILE:brip_test_all> test_fourier )
add_test( NAME brip_test_homography COMMAND $<TARGET_FILE:brip_test_all> test_homography )
add_test( NAME brip_test_lucas_kanade COMMAND $<TARGET_FILE:brip_test_all> test_lucas_kanade )
add_test( NAME brip_test_Horn_Schunck COMMAND $<TARGET_FILE:brip_test_all> test_Horn_Schunck )
add_test( NAME brip_test_quadtree COMMAND $<TARGET_FILE:brip_test_all> test_quadtree )
add_test( NAME brip_test_max_scale_response COMMAND $<TARGET_FILE:brip_test_all> test_max_scale_response )
add_test( NAME brip_test_mask COMMAND $<TARGET_FILE:brip_test_all> test_mask )
add_test( NAME brip_test_label_equivalence COMMAND $<TARGET_FILE:brip_test_all> test_label_equivalence )
add_test( NAME brip_nitf_ops COMMAND $<TARGET_FILE:brip_test_all> test_nitf_ops )
add_test( NAME brip_phase_correlation COMMAND $<TARGET_FILE:brip_test_all> test_phase_correlation )
if(SEGFAULT_FIXED)
add_test( NAME brip_test_extrema COMMAND $<TARGET_FILE:brip_test_all> test_extrema )
add_test( NAME brip_test_filter_bank COMMAND $<TARGET_FILE:brip_test_all> test_filter_bank )
add_test( NAME brip_test_gain_offset_solver COMMAND $<TARGET_FILE:brip_test_all> test_gain_offset_solver )
endif()

add_executable( brip_test_include test_include.cxx )
target_link_libraries( brip_test_include brip )
add_executable( brip_test_template_include test_template_include.cxx )
target_link_libraries( brip_test_template_include brip )
