# contrib/brl/bseg/boxm2_volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/volm )

ADD_EXECUTABLE( boxm2_volm_prepare_location_hypotheses boxm2_volm_prepare_location_hypotheses.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_prepare_location_hypotheses boxm2_volm vbl vgl  )

IF(OPENCL_FOUND)

ADD_EXECUTABLE( boxm2_volumetric_matcher_compose boxm2_volumetric_matcher_compose.cxx )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_compose boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_p0 boxm2_volumetric_matcher_p0.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_p0 boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_p1 boxm2_volumetric_matcher_p1.cxx)
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_p1 boxm2_volm vbl vgl volm depth_map bvrml)

ENDIF(OPENCL_FOUND)
