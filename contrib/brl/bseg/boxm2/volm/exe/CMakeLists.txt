# contrib/brl/bseg/boxm2_volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/volm )

ADD_EXECUTABLE( boxm2_volm_prepare_location_hypotheses boxm2_volm_prepare_location_hypotheses.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_prepare_location_hypotheses boxm2_volm vbl vgl  )

IF(OPENCL_FOUND)

ADD_EXECUTABLE( boxm2_volumetric_matcher_compose boxm2_volumetric_matcher_compose.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_compose boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_rationale boxm2_volumetric_matcher_rationale.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_rationale boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_visualize_top_indices boxm2_volumetric_visualize_top_indices.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_visualize_top_indices boxm2_volm volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_integration boxm2_volumetric_matcher_integration.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_integration boxm2_volm vbl vgl volm depth_map bvrml)

ENDIF(OPENCL_FOUND)
