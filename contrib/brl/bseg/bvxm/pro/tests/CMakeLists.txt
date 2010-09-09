
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm/pro)

ADD_EXECUTABLE( bvxm_pro_test_all
  test_driver.cxx
  test_bvxm_roi_init_process.cxx
  test_bvxm_illum_index_process.cxx
  test_bvxm_create_voxel_world_process.cxx
  test_bvxm_normalize_image_process.cxx
  test_bvxm_create_normalized_image_process.cxx
  test_bvxm_change_detection_display_process.cxx
  test_bvxm_rpc_registration_process.cxx
  test_bvxm_create_local_rpc_process.cxx
  test_bvxm_create_synth_lidar_data_process.cxx
  test_bvxm_update_lidar_process.cxx
)

TARGET_LINK_LIBRARIES( bvxm_pro_test_all testlib bvxm_pro bvxm bvxm_grid_io vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vul vil vil_io vil_pro vgl_algo vnl vgl vbl vcl )

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/change_display_params.xml ${CMAKE_CURRENT_BINARY_DIR}/change_display_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/two_box_synth.tif ${CMAKE_CURRENT_BINARY_DIR}/two_box_synth.tif COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_withICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_withICHIPB.nitf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_image.png ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_image.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_camera.rpb ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_camera.rpb COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/update_edges_parameters.xml ${CMAKE_CURRENT_BINARY_DIR}/update_edges_parameters.xml )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/lidar_params.xml ${CMAKE_CURRENT_BINARY_DIR}/lidar_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/synth_test_params.xml ${CMAKE_CURRENT_BINARY_DIR}/synth_test_params.xml COPYONLY )

ADD_TEST( bvxm_pro_change_detection_display_process     ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_change_detection_display_process )
ADD_TEST( bvxm_pro_test_roi_init_process                ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_roi_init_process )
ADD_TEST( bvxm_pro_test_illum_index_process             ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_illum_index_process )
ADD_TEST( bvxm_pro_test_create_voxel_world_process      ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_create_voxel_world_process )
ADD_TEST( bvxm_pro_test_bvxm_normalize_image_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_normalize_image_process )
ADD_TEST( bvxm_pro_test_bvxm_create_normalized_image_process ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_create_normalized_image_process )
ADD_TEST( bvxm_pro_test_rpc_registration_process        ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_rpc_registration_process ${CMAKE_CURRENT_SOURCE_DIR} )
ADD_TEST( bvxm_pro_test_create_local_rpc_process        ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_create_local_rpc_process )
ADD_TEST( bvxm_pro_test_create_synth_lidar_data_process ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_create_synth_lidar_data_process )
ADD_TEST( bvxm_pro_test_update_lidar_process            ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_update_lidar_process ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_EXECUTABLE( bvxm_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_pro_test_include bvxm_pro )
