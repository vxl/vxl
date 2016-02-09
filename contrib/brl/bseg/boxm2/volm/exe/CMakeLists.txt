# contrib/brl/bseg/boxm2_volm/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bbas/bkml )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2/volm )


add_executable( boxm2_volm_prepare_location_hypotheses boxm2_volm_prepare_location_hypotheses.cxx )
target_link_libraries( boxm2_volm_prepare_location_hypotheses boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl  )

add_executable( boxm2_volumetric_create_prob_map boxm2_volumetric_create_prob_map.cxx )
target_link_libraries( boxm2_volumetric_create_prob_map boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm depth_map )

add_executable( boxm2_volumetric_evaluate_roi boxm2_volumetric_evaluate_roi.cxx )
target_link_libraries( boxm2_volumetric_evaluate_roi boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm bvrml)

add_executable( boxm2_volumetric_evaluate_roi_max_score boxm2_volumetric_evaluate_roi_max_score.cxx )
target_link_libraries( boxm2_volumetric_evaluate_roi_max_score boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm bvrml)

add_executable( boxm2_volumetric_render_index_depth_img boxm2_volumetric_render_index_depth_img.cxx )
target_link_libraries( boxm2_volumetric_render_index_depth_img boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm )


add_executable( boxm2_volm_segment_index boxm2_volm_segment_index.cxx )
target_link_libraries( boxm2_volm_segment_index boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm )

add_executable( boxm2_pass0_compile_results boxm2_pass0_compile_results.cxx)
target_link_libraries( boxm2_pass0_compile_results boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm )

add_executable( boxm2_volumetric_create_candidate_list boxm2_volumetric_create_candidate_list.cxx )
target_link_libraries( boxm2_volumetric_create_candidate_list boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm ${VXL_LIB_PREFIX}vil bkml)

add_executable( boxm2_volumetric_pre_process_p1_integration boxm2_volumetric_pre_process_p1_integration.cxx )
target_link_libraries( boxm2_volumetric_pre_process_p1_integration boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm )

add_executable( boxm2_volumetric_post_process_p1_integration boxm2_volumetric_post_process_p1_integration.cxx )
target_link_libraries( boxm2_volumetric_post_process_p1_integration boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm )

add_executable( boxm2_volm_prepare_xyz_images_with_classes2_forts boxm2_volm_prepare_xyz_images_with_classes2_forts.cxx )
target_link_libraries( boxm2_volm_prepare_xyz_images_with_classes2_forts boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl  )

add_executable( boxm2_volm_get_best_matcher_from_cand_list boxm2_volm_get_best_matcher_from_cand_list.cxx )
target_link_libraries( boxm2_volm_get_best_matcher_from_cand_list boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm ${VXL_LIB_PREFIX}vul bkml)

add_executable( boxm2_volumetric_pinpoint boxm2_volumetric_pinpoint.cxx )
target_link_libraries( boxm2_volumetric_pinpoint boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpgl bkml )

if(OPENCL_FOUND)

add_executable( boxm2_volumetric_matcher_p0 boxm2_volumetric_matcher_p0.cxx)
target_link_libraries( boxm2_volumetric_matcher_p0 boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm depth_map bvrml)

add_executable( boxm2_volumetric_matcher_p1 boxm2_volumetric_matcher_p1.cxx)
target_link_libraries( boxm2_volumetric_matcher_p1 boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm depth_map bvrml)

add_executable( boxm2_volumetric_matcher_p1_integration boxm2_volumetric_matcher_p1_integration.cxx)
target_link_libraries( boxm2_volumetric_matcher_p1_integration boxm2_volm ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl volm depth_map bvrml ${VXL_LIB_PREFIX}vpl)

endif()
