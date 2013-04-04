# contrib/brl/bseg/boxm2_volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/bkml )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/volm )

ADD_EXECUTABLE( boxm2_volm_prepare_location_hypotheses boxm2_volm_prepare_location_hypotheses.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_prepare_location_hypotheses boxm2_volm vbl vgl  )

ADD_EXECUTABLE( boxm2_volumetric_create_prob_map boxm2_volumetric_create_prob_map.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_create_prob_map boxm2_volm vbl vgl volm depth_map )

ADD_EXECUTABLE( boxm2_volumetric_evaluate_roi boxm2_volumetric_evaluate_roi.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_evaluate_roi boxm2_volm vbl vgl volm bvrml)

ADD_EXECUTABLE( boxm2_volumetric_evaluate_roi_max_score boxm2_volumetric_evaluate_roi_max_score.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_evaluate_roi_max_score boxm2_volm vbl vgl volm bvrml)

ADD_EXECUTABLE( boxm2_volumetric_render_index_depth_img boxm2_volumetric_render_index_depth_img.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_render_index_depth_img boxm2_volm vbl vgl volm )

ADD_EXECUTABLE( boxm2_volumetric_pre_process_p1_integration boxm2_volumetric_pre_process_p1_integration.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_pre_process_p1_integration boxm2_volm vbl vgl volm )

ADD_EXECUTABLE( boxm2_volumetric_post_process_p1_integration boxm2_volumetric_post_process_p1_integration.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_post_process_p1_integration boxm2_volm vbl vgl volm )

IF(OPENCL_FOUND)

ADD_EXECUTABLE( boxm2_volumetric_matcher_p0 boxm2_volumetric_matcher_p0.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_p0 boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_p1 boxm2_volumetric_matcher_p1.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_p1 boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_p1_integration boxm2_volumetric_matcher_p1_integration.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_p1_integration boxm2_volm vbl vgl volm depth_map bvrml)

ENDIF(OPENCL_FOUND)
