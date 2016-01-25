include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bseg/brec )
if( EXPAT_FOUND )
 if(EXPATPP_FOUND)
 add_executable( brec_test_all
  test_driver.cxx
  test_brec_densities.cxx
  test_param_estimation.cxx
  test_brec_parts.cxx
  test_brec_hierarchy_builder.cxx
  test_brec_hierarchy_detector.cxx
  test_brec_hierarchy_detector_w_models.cxx
  test_brec_part_hierarchy_learner.cxx
  test_brec_hierarchy_edge.cxx
  test_brec_hierarchy.cxx
  test_brec_hierarchy_detector2.cxx
)

target_link_libraries( brec_test_all brec testlib vpgl vil_pro vil_io vnl vil vul vbl expat expatpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/digits_small.png    ${CMAKE_CURRENT_BINARY_DIR}/digits_small.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_cropped.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_gt_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_gt_cropped.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_cropped.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_mask_cropped.bin    ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_mask_cropped.bin COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_prob_map_cropped.tiff   ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_prob_map_cropped.tiff COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/out_mog_0_std_dev_img_cropped.tiff    ${CMAKE_CURRENT_BINARY_DIR}/out_mog_0_std_dev_img_cropped.tiff COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/out_mog_0_mean_img_cropped.tiff    ${CMAKE_CURRENT_BINARY_DIR}/out_mog_0_mean_img_cropped.tiff COPYONLY )

add_test( NAME brec_test_densities COMMAND $<TARGET_FILE:brec_test_all> test_brec_densities )
 add_test( NAME brec_test_param_estimation COMMAND $<TARGET_FILE:brec_test_all> test_param_estimation )
 add_test( NAME brec_test_parts COMMAND $<TARGET_FILE:brec_test_all> test_brec_parts )
 add_test( NAME brec_test_hierarchy_builder COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy_builder )
 add_test( NAME brec_test_hierarchy_detector COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy_detector )
 add_test( NAME brec_test_hierarchy_detector_w_models COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy_detector_w_models )
 add_test( NAME brec_test_part_hierarchy_learner COMMAND $<TARGET_FILE:brec_test_all> test_brec_part_hierarchy_learner )
 add_test( NAME brec_test_hierarchy_edge COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy_edge )
 add_test( NAME brec_test_hierarchy COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy )
 if(SEGFAULT_FIXED)
 add_test( NAME brec_test_hierarchy_detector2 COMMAND $<TARGET_FILE:brec_test_all> test_brec_hierarchy_detector2 )
  endif()
 endif()
endif()
add_executable( brec_test_include test_include.cxx )
target_link_libraries( brec_test_include brec )
