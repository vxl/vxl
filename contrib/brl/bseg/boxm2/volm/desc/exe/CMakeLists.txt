# contrib/brl/bseg/boxm2/volm/desc/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas/volm )

add_executable( boxm2_volm_desc_ex_create_indexer boxm2_volm_desc_ex_create_indexer.cxx )
target_link_libraries( boxm2_volm_desc_ex_create_indexer boxm2_volm_desc )

add_executable( boxm2_volm_desc_ex_matcher boxm2_volm_desc_ex_matcher.cxx )
target_link_libraries( boxm2_volm_desc_ex_matcher boxm2_volm_desc )
