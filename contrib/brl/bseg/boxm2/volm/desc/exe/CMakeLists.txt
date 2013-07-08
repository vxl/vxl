# contrib/brl/bseg/boxm2/volm/desc/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/volm )

ADD_EXECUTABLE( boxm2_volm_desc_ex_create_indexer boxm2_volm_desc_ex_create_indexer.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_desc_ex_create_indexer boxm2_volm_desc )

ADD_EXECUTABLE( boxm2_volm_desc_ex_matcher boxm2_volm_desc_ex_matcher.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_desc_ex_matcher boxm2_volm_desc )