# contrib/brl/bbas/volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/ )

ADD_EXECUTABLE( volm_dummy_matcher
                volm_dummy_matcher.cxx
              )

ADD_EXECUTABLE( volm_query_creation
                volm_query_creation.cxx
              )

ADD_EXECUTABLE( volm_generate_hypos
                volm_generate_hypos.cxx
              )

ADD_EXECUTABLE( volm_evaluate_output_tiles
                volm_evaluate_output_tiles.cxx
              )

ADD_EXECUTABLE( volm_construct_cam_space_query 
                volm_construct_cam_space_query.cxx
              )

ADD_EXECUTABLE( volm_evaluate
                volm_evaluate.cxx
              )

TARGET_LINK_LIBRARIES( volm_dummy_matcher volm depth_map )
TARGET_LINK_LIBRARIES( volm_query_creation volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_generate_hypos volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_evaluate_output_tiles volm depth_map bvrml )
TARGET_LINK_LIBRARIES( volm_construct_cam_space_query volm vbl vgl depth_map bvrml)
TARGET_LINK_LIBRARIES( volm_evaluate volm depth_map bvrml )
