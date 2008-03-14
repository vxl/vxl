
INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/bvxm )

ADD_EXECUTABLE( bvxm_pro_test_all
  test_driver.cxx
  test_bvxm_roi_init_process.cxx
  test_bvxm_illum_index_process.cxx
  test_bvxm_create_voxel_world_process.cxx
  test_bvxm_normalize_image_process.cxx
  test_bvxm_change_detection_display_process.cxx
  test_bvxm_generate_edge_map_process.cxx
  test_bvxm_rpc_registration_process.cxx
)

TARGET_LINK_LIBRARIES( bvxm_pro_test_all testlib bvxm_pro bvxm vcl vbl vpgl_algo vpgl vpgl_io vpgl_bpro bprb brdb vil vil_io vil_bpro)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/change_display_params.xml ${CMAKE_CURRENT_BINARY_DIR}/change_display_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/edge_map_params.xml ${CMAKE_CURRENT_BINARY_DIR}/edge_map_params.xml COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/two_box_synth.tif ${CMAKE_CURRENT_BINARY_DIR}/two_box_synth.tif COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_image.png ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_image.png COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_camera.rpb ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_camera.rpb COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_parameters.xml ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_parameters.xml )

ADD_TEST( bvxm_change_detection_display_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_change_detection_display_process )
ADD_TEST( bvxm_test_roi_init_process       ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_roi_init_process )
ADD_TEST( bvxm_test_illum_index_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_illum_index_process )
ADD_TEST( bvxm_test_create_voxel_world_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_create_voxel_world_process )
ADD_TEST( bvxm_test_bvxm_normalize_image_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_normalize_image_process )
ADD_TEST( bvxm_test_generate_edge_map_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_generate_edge_map_process )
ADD_TEST( bvxm_test_rpc_registration_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_pro_test_all   test_bvxm_rpc_registration_process )
