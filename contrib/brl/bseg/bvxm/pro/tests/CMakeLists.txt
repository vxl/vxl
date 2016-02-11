
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm/pro)

add_executable( bvxm_pro_test_all
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

target_link_libraries( bvxm_pro_test_all ${VXL_LIB_PREFIX}testlib bvxm_pro bvxm bvxm_grid_io ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io vpgl_pro bprb brdb ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io vil_pro ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/roi_params.xml ${CMAKE_CURRENT_BINARY_DIR}/roi_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params.xml ${CMAKE_CURRENT_BINARY_DIR}/world_model_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/world_model_params_lvcs.txt ${CMAKE_CURRENT_BINARY_DIR}/world_model_params_lvcs.txt COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/change_display_params.xml ${CMAKE_CURRENT_BINARY_DIR}/change_display_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/two_box_synth.tif ${CMAKE_CURRENT_BINARY_DIR}/two_box_synth.tif COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_noICHIPB.nitf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/po_39928_pan_0000000_chip_700_700_withICHIPB.nitf ${CMAKE_CURRENT_BINARY_DIR}/po_39928_pan_0000000_chip_700_700_withICHIPB.nitf COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_image.png ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_image.png COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rpc_registration_camera.rpb ${CMAKE_CURRENT_BINARY_DIR}/rpc_registration_camera.rpb COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/update_edges_parameters.xml ${CMAKE_CURRENT_BINARY_DIR}/update_edges_parameters.xml )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lidar_params.xml ${CMAKE_CURRENT_BINARY_DIR}/lidar_params.xml COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/synth_test_params.xml ${CMAKE_CURRENT_BINARY_DIR}/synth_test_params.xml COPYONLY )

add_test( NAME bvxm_pro_change_detection_display_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_change_detection_display_process )
add_test( NAME bvxm_pro_test_roi_init_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_roi_init_process )
add_test( NAME bvxm_pro_test_illum_index_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_illum_index_process )
add_test( NAME bvxm_pro_test_create_voxel_world_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_create_voxel_world_process )
add_test( NAME bvxm_pro_test_bvxm_normalize_image_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_normalize_image_process )
add_test( NAME bvxm_pro_test_bvxm_create_normalized_image_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_create_normalized_image_process )
add_test( NAME bvxm_pro_test_rpc_registration_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_rpc_registration_process ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( NAME bvxm_pro_test_create_local_rpc_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_create_local_rpc_process )
add_test( NAME bvxm_pro_test_create_synth_lidar_data_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_create_synth_lidar_data_process )
add_test( NAME bvxm_pro_test_update_lidar_process COMMAND $<TARGET_FILE:bvxm_pro_test_all>   test_bvxm_update_lidar_process ${CMAKE_CURRENT_SOURCE_DIR} )

add_executable( bvxm_pro_test_include test_include.cxx )
target_link_libraries( bvxm_pro_test_include bvxm_pro )
