#contrib/brl/bbas/volm/conf/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas/ )

ADD_EXECUTABLE( volm_conf_create_land_map_indexer
                volm_conf_create_land_map_indexer.cxx
              )

ADD_EXECUTABLE( volm_conf_create_2d_index
                volm_conf_create_2d_index.cxx
              )

ADD_EXECUTABLE( volm_conf_visualize_index
                volm_conf_visualize_index.cxx
              )

TARGET_LINK_LIBRARIES( volm_conf_create_land_map_indexer volm volm_conf )
TARGET_LINK_LIBRARIES( volm_conf_create_2d_index volm volm_conf )
TARGET_LINK_LIBRARIES( volm_conf_visualize_index volm volm_conf )