INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/brec )

ADD_EXECUTABLE( brec_test_all
  test_driver.cxx
  test_brec_densities.cxx
  test_param_estimation.cxx
  test_brec_parts.cxx
  test_brec_hierarchy_builder.cxx
  test_brec_hierarchy_detector.cxx
  test_brec_hierarchy_detector_w_models.cxx
  test_brec_part_hierarchy_learner.cxx
)

TARGET_LINK_LIBRARIES( brec_test_all brec testlib vcl vbl vpgl vil_pro )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/digits_small.png    ${CMAKE_CURRENT_BINARY_DIR}/digits_small.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_cropped.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/normalized0_gt_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/normalized0_gt_cropped.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_cropped.png    ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_cropped.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_mask_cropped.bin    ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_mask_cropped.bin COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/test_view_0_prob_map_cropped.tiff   ${CMAKE_CURRENT_BINARY_DIR}/test_view_0_prob_map_cropped.tiff COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/out_mog_0_std_dev_img_cropped.tiff    ${CMAKE_CURRENT_BINARY_DIR}/out_mog_0_std_dev_img_cropped.tiff COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/out_mog_0_mean_img_cropped.tiff    ${CMAKE_CURRENT_BINARY_DIR}/out_mog_0_mean_img_cropped.tiff COPYONLY )

ADD_TEST( brec_test_densities     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_densities )
ADD_TEST( brec_test_param_estimation     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_param_estimation )
ADD_TEST( brec_test_parts     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_parts )
ADD_TEST( brec_test_hierarchy_builder     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_hierarchy_builder )
ADD_TEST( brec_test_hierarchy_detector     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_hierarchy_detector )
ADD_TEST( brec_test_hierarchy_detector_w_models     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_hierarchy_detector_w_models )
ADD_TEST( brec_test_part_hierarchy_learner     ${EXECUTABLE_OUTPUT_PATH}/brec_test_all test_brec_part_hierarchy_learner )

ADD_EXECUTABLE( brec_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brec_test_include brec )
