# contrib/brl/bseg/boxm2/volm/conf/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/volm )

ADD_EXECUTABLE( boxm2_volm_conf_matcher_exe
                boxm2_volm_conf_matcher_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volm_conf_matcher_exe boxm2_volm_conf )

ADD_EXECUTABLE( boxm2_volm_conf_create_prob_map
                boxm2_volm_conf_create_prob_map.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volm_conf_create_prob_map boxm2_volm_conf )

ADD_EXECUTABLE( boxm2_volm_conf_create_candidate_region
                boxm2_volm_conf_create_candidate_region.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volm_conf_create_candidate_region boxm2_volm_conf )
