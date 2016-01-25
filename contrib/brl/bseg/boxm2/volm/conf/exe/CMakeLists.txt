# contrib/brl/bseg/boxm2/volm/conf/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/volm )

add_executable( boxm2_volm_conf_matcher_exe
                boxm2_volm_conf_matcher_exe.cxx
              )
target_link_libraries( boxm2_volm_conf_matcher_exe boxm2_volm_conf )

add_executable( boxm2_volm_conf_create_prob_map
                boxm2_volm_conf_create_prob_map.cxx
              )
target_link_libraries( boxm2_volm_conf_create_prob_map boxm2_volm_conf )

add_executable( boxm2_volm_conf_create_candidate_region
                boxm2_volm_conf_create_candidate_region.cxx
              )
target_link_libraries( boxm2_volm_conf_create_candidate_region boxm2_volm_conf )
